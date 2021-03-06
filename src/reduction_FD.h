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

#include"reduction.h"
#include"bedslope.h"

class lexer;
class fdm;
class ghostcell;

using namespace std;

#ifndef REDUCTION_FD_H_
#define REDUCTION_FD_H_

class reduction_FD :  public reduction, public bedslope
{
public:
    reduction_FD(lexer*);
    virtual ~reduction_FD();

	virtual double start(lexer*, fdm*,ghostcell*);

private:

    double u_abs,u_plus,dist;
    double uvel, vvel;
    double teta, alpha, beta, gamma;
    double phi;
};

#endif


