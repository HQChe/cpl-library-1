/*!
 * Exact calculation of the overlap volume of spheres and mesh elements.
 * http://dx.doi.org/10.1016/j.jcp.2016.02.003
 *
 * Copyright (C) 2015-2017 Severin Strobl <severin.strobl@fau.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE normal_newell
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include "overlap.hpp"

BOOST_AUTO_TEST_CASE(normal_newell) {
	std::array<vector_t, 3> points = {{
		vector_t(-0.8482081444352685, -0.106496132943784, -0.5188463331100054),
		vector_t(-0.8482081363047198, -0.1064961977010221, -0.5188463331100054),
		vector_t(-0.8482081363047198, -0.106496132943784, -0.5188463464017972)
	}};

	vector_t center = 1.0 / 3.0 * std::accumulate(points.begin(),
		points.end(), vector_t::Zero().eval());

	vector_t normal(detail::normalNewell(points.begin(), points.end(), center));

	vector_t expected(0.8482081353353663, 0.1064961653160474,
		0.5188463413419023);

	BOOST_CHECK_MESSAGE((normal - expected).norm() <
		std::numeric_limits<scalar_t>::epsilon(),
		"Invalid normal generated: " << normal);
}
