/*--------------------------------------------------------------------
REEF3D
Copyright 2008-2020 Hans Bihs

This file is part of REEF3D.

REEF3D is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------
--------------------------------------------------------------------*/

#include"fnpf_state.h"
#include"lexer.h"
#include"fdm_fnpf.h"
#include"ghostcell.h"
#include<iostream>
#include<fstream>
#include<sys/stat.h>
#include<sys/types.h>

void fnpf_state::header(lexer *p, fdm_fnpf *c, ghostcell *pgc)
{
    
    //cout<<p->mpirank<<" HEADER OUT"<<endl;
	
    // file name
    filename_header(p,c,pgc);
    
    //cout<<p->mpirank<<" HEADER NAME: "<<name<<endl;
    
    // open file
	headout.open(name, ios::binary);
    
  
    // ini write    
    iin=p->origin_i;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->origin_j;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->origin_k;
    headout.write((char*)&iin, sizeof (int));


    ffn=float(p->originx);
    headout.write((char*)&ffn, sizeof (float));
    
    ffn=float(p->originy);
    headout.write((char*)&ffn, sizeof (float));
    
    ffn=float(p->originz);
    headout.write((char*)&ffn, sizeof (float));
  
    
    iin=p->knox;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->knoy;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->knoz+1;
    headout.write((char*)&iin, sizeof (int));
    
    
    iin=p->nb1;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->nb2;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->nb3;
    headout.write((char*)&iin, sizeof (int));
    
    iin=p->nb4;
    headout.write((char*)&iin, sizeof (int));
    
    
    //
    ILOOP
    {
    ffn=float(p->XP[IP]);
    headout.write((char*)&ffn, sizeof (float));
    } 
    
    JLOOP
    {
    ffn=float(p->YP[JP]);
    headout.write((char*)&ffn, sizeof (float));
    } 
    
    FKLOOP
    {
    ffn=float(p->ZN[KP]);
    headout.write((char*)&ffn, sizeof (float));
    } 
    
    
    SLICELOOP4
    {
    ffn=float(c->bed(i,j));
    headout.write((char*)&ffn, sizeof (float));
    } 
    
    headout.close();
}


