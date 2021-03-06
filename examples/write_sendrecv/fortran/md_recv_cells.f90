program md_cpl_example
    use cpl, only : CPL_init, CPL_setup_md, & 
                    CPL_get_olap_limits, CPL_my_proc_portion, &
                    CPL_get_no_cells, CPL_send, CPL_recv, &
					CPL_overlap, CPL_finalize, cpl_write_arrays
    use mpi
    implicit none

    logical :: recv_flag,send_flag, NO_ERROR
    integer :: n, ixyz,i,j,k,ii,jj,kk,ierr,errorcode
    integer :: rank, nprocs_realm
    integer :: CART_COMM, MD_COMM
    integer, parameter :: md_realm=2
    integer, dimension(3) :: npxyz, Ncells
    integer, dimension(6) :: portion, limits
    double precision, dimension(3)  :: xyzL, xyz_orig
    double precision, dimension(:), allocatable  :: read_array, sent_array
    double precision, dimension(:,:,:,:), allocatable  :: recv_array, send_array, test_array

    !Initialise MPI
    call MPI_Init(ierr)

    !Create MD Comm by spliting world
    call CPL_init(md_realm, MD_COMM, ierr)

    ! Parameters of the cpu topology (cartesian grid)
    xyzL = (/10.d0, 10.d0, 10.d0/)
    xyz_orig = (/0.d0, 0.d0, 0.d0/)
    npxyz = (/ 2, 2, 2/)

    ! Create communicators and check that number of processors is consistent
    call MPI_Comm_size(MD_COMM, nprocs_realm, ierr) 

    if (nprocs_realm .ne. product(npxyz)) then
        print'(4(a,i6))', "Non-coherent number of processes in MD ", nprocs_realm, & 
                " no equal to ",  npxyz(1), " X ", npxyz(2), " X ", npxyz(3)
        call MPI_Abort(MPI_COMM_WORLD, errorcode, ierr)
    endif

    !Setup cartesian topology
    call MPI_comm_rank(MD_COMM, rank, ierr)
    call MPI_Cart_create(MD_COMM, 3, npxyz, (/.true.,.true.,.true./), & 
                         .true., CART_COMM, ierr)

    !Coupler setup
    call CPL_setup_md(CART_COMM, xyzL, xyz_orig)

    !Get detail for grid
    call CPL_get_olap_limits(limits)
    call CPL_my_proc_portion(limits, portion)
    call CPL_get_no_cells(portion, Ncells)

    !Coupled Recieve and print
    allocate(recv_array(3, Ncells(1), Ncells(2), Ncells(3)))
    recv_array = 0.d0
    call CPL_recv(recv_array, limits, recv_flag)

    ! Check that every processor inside the overlap region receives the cell correctly
    ! number.  
    if (CPL_overlap()) then
        no_error = .true.
        do i = 1, Ncells(1)
        do j = 1, Ncells(2)
        do k = 1, Ncells(3)
            ! -2 indices to match c++ and python indexing in portion and i,j,k
            ii = i + portion(1) - 2
            jj = j + portion(3) - 2
            kk = k + portion(5) - 2

            if ((dble(ii) - recv_array(1,i,j,k)) .gt. 1e-8) then 
                print'(a,2i5,a,i5,a,i6,a,f10.5)', "ERROR -- portion in x: ", portion(1:2), & 
                       " MD rank: ", rank, " cell i: ",ii, & 
                       " recv_array: ", recv_array(1,i,j,k)
                no_error = .false.
            endif
            if ((dble(jj) - recv_array(2,i,j,k)) .gt. 1e-8) then 
                print'(a,2i5,a,i5,a,i6,a,f10.5)', "ERROR -- portion in y: ", portion(3:4), & 
                       " MD rank: ", rank, " cell j: ", jj , & 
                       " recv_array: ", recv_array(2,i,j,k)
                no_error = .false.  
            endif
            if ((dble(kk) - recv_array(3,i,j,k)) .gt. 1e-8) then 
                print'(a,2i5,a,i5,a,i6,a,f10.5)', "ERROR -- portion in z: ", portion(5:6), & 
                       " MD rank: ", rank, " cell k: ", kk , & 
                       " recv_array: ", recv_array(3,i,j,k)
                no_error = .false.
            endif
        enddo
        enddo
        enddo
    endif

    !Block before checking if successful
    call MPI_Barrier(MD_COMM, ierr)
    if (CPL_overlap() .and. no_error) then
        print'(a,a,i2,a)', "MD -- ", "(rank=", rank, ") CELLS HAVE BEEN RECEIVED CORRECTLY."
    endif
    call MPI_Barrier(MPI_COMM_WORLD, ierr)

    !Delete file
    if (rank .eq. 1) then
	    open(2,file='./CPL_post_recv.0000000',status='replace')
        close(2)
    endif

    !Write data here
    call CPL_write_arrays(recv_array, 3, "CPL_post_recv", 1, MD_COMM, limits)
    call MPI_barrier(MD_COMM, ierr)

    !Read data to test
    if (rank .eq. 1) then
        call CPL_get_no_cells(limits, Ncells)
        allocate(read_array(3*Ncells(1)*Ncells(2)*Ncells(3)))
        read_array = 0.d0
	    open(2,file="./CPL_post_recv.0000000", form='unformatted', access='stream',position='rewind')
        read(2) read_array
        close(2)

        !Check array against one that was sent
        allocate(sent_array(3*Ncells(1)*Ncells(2)*Ncells(3)))
        sent_array = 0.d0
	    open(2,file="./CPL_pre_send.0000000", form='unformatted', access='stream',position='rewind')
        read(2) sent_array
        close(2)
        do i =1, size(sent_array)
            if (abs(sent_array(i)-read_array(i)) .gt. 1e-8) then
                print'(a,3i4,i8,a,3f10.5)', "i ", i,j,k,n, "Error -- read_array: ", read_array(i), sent_array(i)
            endif

        enddo

        ! Check that every processor inside the overlap region receives the cell correctly
        ! number.
        allocate(test_array(3,Ncells(1),Ncells(2),Ncells(3)))
        test_array = reshape(read_array, (/Ncells(3), Ncells(2),Ncells(1),3 /))
        n = 0
        do i = 1, Ncells(1)
        do j = 1, Ncells(2)
        do k = 1, Ncells(3)
            n = n + 1
            if (abs(test_array(i,j,k,1)-(i-1)) .gt. 1e-8) then
                print'(a,3i4,i8,a,3f10.5)', "i,j,k,n ", i,j,k,n, "Error -- test_array: ", test_array(k,j,i,:)
            endif

            if (abs(test_array(i,j,k,2)-(j-1)) .gt. 1e-8) then
                print'(a,3i4,i8,a,3f10.5)', "i,j,k,n ", i,j,k,n, "Error -- test_array: ", test_array(k,j,i,:)
            endif

            if (abs(test_array(i,j,k,3)-(k-1)) .gt. 1e-8) then
                print'(a,3i4,i8,a,3f10.5)', "i,j,k,n ", i,j,k,n, "Error -- test_array: ", test_array(k,j,i,:)
            endif

        enddo
        enddo
        enddo
    endif

    !Release all coupler comms 
    call CPL_finalize(ierr)

    !Deallocate arrays and finalise MPI
    deallocate(recv_array)
    call MPI_Comm_free(MD_COMM, ierr)
    call MPI_Comm_free(CART_COMM, ierr)
    call MPI_finalize(ierr)

end program
