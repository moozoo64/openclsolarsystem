/*
	Copyright 2013 Michael William Simmons

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/
#ifndef INITIALSTATE_HPP
#define INITIALSTATE_HPP

#ifndef GLCANVAS_H_
#include "glcanvas.hpp"
#endif // #ifndef GLCANVAS_H_

#ifndef CLMODEL_H
#include "clmodel.hpp"
#endif // #ifndef CLMODEL_H

#include "physicalproperties.hpp"

class InitialState
{
private:
	unsigned long xor128();
public:
	int initialNumParticles;
	int initialNumGrav;
	cl_double4 *initialVelocities;
	cl_double4 *initialPositions;
	GLubyte *initialColorData;
	PhysicalProperties *physicalProperties;
	cl_double initialJulianDate;
	InitialState();
	~InitialState();
	void DeAllocate();
	bool Allocate();
	void SetDefaultBodyColours();
	bool ImportSLF( wxString fileName );
	bool ExportSLF( wxString fileName );
	bool LoadInitialState( wxString fileName );
	bool SaveInitialState( wxString fileName );
	bool CreateRandomInitialConfig();
};

#endif // INITIALSTATE_HPP
