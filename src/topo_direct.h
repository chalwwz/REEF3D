/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2020 Hans Bihs

This file is part of REEF3D.

REEF3D is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTIBILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
Author: Hans Bihs
--------------------------------------------------------------------*/

#include"topo.h"
#include"topo_vel.h"
#include"slice4.h"

using namespace std;

#ifndef TOPO_DIRECT_H_
#define TOPO_DIRECT_H_

class topo_direct : public topo, topo_vel
{
public:
	topo_direct(lexer*, fdm*, ghostcell*,turbulence*);
	virtual ~topo_direct();
	virtual void start(fdm*,lexer*, convection*, ghostcell*,reinitopo*);


private:
	int gcval_topo;
	double starttime;

	double vx,vy,vz;
	double vzmax;
    
    slice4 dh;

};

#endif

