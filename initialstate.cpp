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
#include "global.hpp"
#include "initialstate.hpp"

InitialState::InitialState()
{
	this->initialVelocities = NULL;
	this->initialPositions  = NULL;
	this->initialColorData  = NULL;
	this->physicalProperties = NULL;
	this->initialNumParticles = 0;
	this->initialNumGrav = 0;
}

InitialState::~InitialState()
{
	this->DeAllocate();
}

// Deallocate the arrays
void InitialState::DeAllocate()
{
	if(this->physicalProperties != NULL)
	{
		delete[] this->physicalProperties;
		this->physicalProperties = NULL;
	}
	
	if(this->initialVelocities != NULL)
	{
		delete[] this->initialVelocities;
		this->initialVelocities = NULL;
	}

	if(this->initialPositions != NULL)
	{
		delete[] this->initialPositions;
		this->initialPositions = NULL;
	}
	
	if(this->initialColorData != NULL)
	{
		delete[] this->initialColorData;
		this->initialColorData = NULL;
	}
}

// Allocate the arrays
bool InitialState::Allocate()
{
	if(this->initialNumParticles == 0)
	{
		wxLogError(wxT("initialNumParticles is zero"));
		return false;
	}
	
	this->initialVelocities = new cl_double4[this->initialNumParticles];
	if(this->initialVelocities == NULL)
	{
		wxLogError(wxT("Failed to Allocate initalVelocity"));
		return false;
	}

	this->initialPositions = new cl_double4[this->initialNumParticles];
	if(this->initialPositions == NULL)
	{
		wxLogError(wxT("Failed to Allocate initalPositions"));
		return false;
	}
	
	unsigned int colorSize = this->initialNumParticles * 4 * sizeof(GLubyte);
	this->initialColorData = new GLubyte [colorSize];
	if(this->initialColorData == NULL)
	{
		wxLogError(wxT("Failed to Allocate initialColorData"));
		return false;
	}
	
	this->physicalProperties = new PhysicalProperties[this->initialNumParticles];
	if(this->physicalProperties == NULL)
	{
		wxLogError(wxT("Failed to Allocate physicalProperties"));
		return false;
	}
	
	return true;
}

/* Random Number Generator From Xorshift RNGs by George Marsaglia */
/* http://en.wikipedia.org/wiki/Xorshift */
unsigned long InitialState::xor128()
{
	static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
	unsigned long t;
	t=(x^(x<<11));
	x=y;
	y=z;
	z=w;
	return(w=(w^(w>>19))^(t^(t>>8)));
}

// set the colour of the bodies based on name and if its mass is being used.
void InitialState::SetDefaultBodyColours()
{
	if(this->physicalProperties == NULL || this->initialColorData == NULL)
	{
		wxLogDebug(wxT("SetDefaultBodyColours called with physicalProperties or initialColorData NULL "));
		return;
	}
	
	for(int i=0; i<this->initialNumParticles; i++)
	{
		wxString name = wxString(this->physicalProperties[i].Name);
		
		if(i > this->initialNumGrav)
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 128;
		}else if(name.IsSameAs(wxT("Sun"),false))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("RogueStar"),false))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("RogueDwarf"),false))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 192;
			this->initialColorData[i*4+2] = 192;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("RoguePlanet"),false))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Mercury"),false))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Venus")))
		{
			this->initialColorData[i*4+0] = 192;
			this->initialColorData[i*4+1] = 192;
			this->initialColorData[i*4+2] = 192;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Earth")))
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
			
		}else if(name.IsSameAs(wxT("Moon")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
			
		}else if(name.IsSameAs(wxT("Mars")))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Jupiter")))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Saturn")))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Uranus")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Neptune")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Pluto")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Ceres")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Pallas")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Vesta")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("Juno")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}else if(name.IsSameAs(wxT("C2013A1")))
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}else
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
	}
}

// write the initial state out to a file in binary format
bool InitialState::SaveInitialState(wxString fileName)
{
	bool success = false;
	try
	{
		wxFile stateFile;
		stateFile.Create(fileName,true);
		stateFile.Write(&this->initialNumParticles,sizeof(int));
		stateFile.Write(&this->initialNumGrav,sizeof(int));
		stateFile.Write(&this->initialJulianDate,sizeof(cl_double));
		stateFile.Write(this->initialPositions,this->initialNumParticles * sizeof(cl_double4));
		stateFile.Write(this->initialVelocities,this->initialNumParticles * sizeof(cl_double4));
		stateFile.Write(this->initialColorData,this->initialNumParticles * 4 * sizeof(GLubyte));
		stateFile.Write(this->physicalProperties,this->initialNumParticles * sizeof(PhysicalProperties));
		stateFile.Close();
		success = true;
	}catch(int e)
	{
		wxLogError(wxT("Save Failed"));
	}
	
	return success;
}

// loads the initial state in binary format from a file
bool InitialState::LoadInitialState(wxString fileName)
{
	wxFile stateFile;
	if(stateFile.Open(fileName))
	{
		stateFile.Read(&this->initialNumParticles,sizeof(int));
		stateFile.Read(&this->initialNumGrav,sizeof(int));
		stateFile.Read(&this->initialJulianDate,sizeof(cl_double));
		
		this->DeAllocate();
		if(!this->Allocate())
		{
			throw -1;
		}

		stateFile.Read(this->initialPositions,this->initialNumParticles * sizeof(cl_double4));
		stateFile.Read(this->initialVelocities,this->initialNumParticles * sizeof(cl_double4));
		stateFile.Read(this->initialColorData,this->initialNumParticles * 4 * sizeof(GLubyte));
		stateFile.Read(this->physicalProperties,this->initialNumParticles * sizeof(PhysicalProperties));
		stateFile.Close();
		
		return true;
	}
	return false;
}

// Exports the initial state in Solex SLF file format
bool InitialState::ExportSLF(wxString fileName)
{
	wxString message;
	message.Printf("Saving %s",fileName.c_str());
	wxProgressDialog progressBar(message, wxT("Saving"), this->initialNumParticles, NULL, wxPD_AUTO_HIDE );
	
	wxFileOutputStream fileOutputStream(fileName);
	wxTextOutputStream exportSLFTextOut(fileOutputStream);
	wxString line;
	line.Printf("%.16G\n 3 ",this->initialJulianDate);
	exportSLFTextOut.WriteString(line);
	for(int i=0; i< this->initialNumParticles; i++)
	{
		line.Printf("%.16G %.16G %.16G %.16G# %s\n",this->physicalProperties[i].Mass,this->physicalProperties[i].Radius,this->physicalProperties[i].AbsoluteMagnitude,this->physicalProperties[i].RelativisticParameter,this->physicalProperties[i].Name);
		exportSLFTextOut.WriteString(line);
		line.Printf("%.16E %.16E %.16E\n",this->initialPositions[i].x,this->initialPositions[i].y,this->initialPositions[i].z);
		exportSLFTextOut.WriteString(line);
		line.Printf("%.16E %.16E %.16E\n",this->initialVelocities[i].x,this->initialVelocities[i].y,this->initialVelocities[i].z);
		exportSLFTextOut.WriteString(line);
		
		if( i % 100 == 0)
		{
			message.Printf("%d",i);
			progressBar.Update(i,message);
		}
	}
	return true;
}

// imports a Solex SLF formated file
bool InitialState::ImportSLF(wxString fileName)
{
	this->initialNumParticles = 1000000;
	this->initialNumGrav = 16;
	
	wxString message;
	message.Printf("Loading %s",fileName.c_str());
	wxProgressDialog progressBar(message, wxT("Loading"), this->initialNumParticles, NULL, wxPD_AUTO_HIDE );
	
	this->DeAllocate();
	if(!this->Allocate())
	{
		return false;
	}
	
	wxFileInputStream initialConditionsfileInputStream(fileName);
	wxTextInputStream initialConditions(initialConditionsfileInputStream);

	if(initialConditionsfileInputStream.Eof())
	{
		wxLogDebug(wxT("File is Empty"));
		return false;
	}

	progressBar.Update(0,wxT("Loading"));

	long bodiesReadCount = 0L;
	wxString FirstLine = initialConditions.ReadLine();
	double time;
	FirstLine.ToDouble(&time);
	this->initialJulianDate = time;
	
	this->SetDefaultBodyColours();
	
	int moon = -1;
	int earth = -1;
	for(int i=0; i< this->initialNumParticles; i++)
	{
		this->initialPositions[i].w = 0.2 * double(xor128()/double(ULONG_MAX));
		this->initialPositions[i].x = double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));
		this->initialPositions[i].y = double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));
		this->initialPositions[i].z = double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));

		this->initialVelocities[i].x=0.0;
		this->initialVelocities[i].y=0.0;
		this->initialVelocities[i].z=0.0;
		this->initialVelocities[i].w=0.0;

		if(!initialConditionsfileInputStream.Eof())
		{
			double mass,radius,absoluteMagnitude,relativisticParameter,xPos,yPos,zPos,xVel,yVel,zVel;

			if(initialConditionsfileInputStream.Eof())
			{
				wxLogDebug(wxT("Read in %ld"), bodiesReadCount);
				break;
			}
			wxString line = initialConditions.ReadLine();
			wxStringTokenizer *lineTokenizer = new wxStringTokenizer(line);
			if(i == 0)
			{
				lineTokenizer->GetNextToken();
			}

			// Get Mass
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected mass"));
				break;
			}
			wxString massToken = lineTokenizer->GetNextToken();
			if(!massToken.ToDouble(&mass))
			{
				wxLogDebug(wxT("Invalid mass %s"), massToken);
				return false;
			}

			// Get Radius
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected radius"));
				break;
			}
			wxString radiusToken = lineTokenizer->GetNextToken();
			if(!radiusToken.ToDouble(&radius))
			{
				wxLogDebug(wxT("Invalid radius %s"), radiusToken);
				return false;
			}

			// Get absolute magnitude
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected absolute magnitude"));
				break;
			}
			wxString absoluteMagnitudeToken = lineTokenizer->GetNextToken();
			if(!absoluteMagnitudeToken.ToDouble(&absoluteMagnitude))
			{
				wxLogDebug(wxT("Invalid absolute magnitude %s"), absoluteMagnitudeToken);
				return false;
			}
			
			// Get relativistic parameter
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected relativistic parameter"));
				break;
			}
			wxString relativisticParameterToken = lineTokenizer->GetNextToken();
			if(relativisticParameterToken.EndsWith(wxT("#"),NULL))
			{
				relativisticParameterToken.RemoveLast();
			}
			if(!relativisticParameterToken.ToDouble(&relativisticParameter))
			{
				wxLogDebug(wxT("Invalid relativisticParameter"),relativisticParameterToken);
				return false;
			}
			// Get name
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected name"));
				break;
			}
			wxString nameToken = lineTokenizer->GetNextToken();
			
			if(nameToken.IsSameAs(wxT("Earth")))
			{
				earth = i;
			}else if(nameToken.IsSameAs(wxT("Moon")))
			{
				moon = i;
			}
			
			// Next Line
			if(initialConditionsfileInputStream.Eof())
			{
				wxLogDebug(wxT("Expected xPos. Read in %ld"), bodiesReadCount);
				break;
			}
			line = initialConditions.ReadLine();
			lineTokenizer = new wxStringTokenizer(line);

			// Get xPos
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected x Position"));
				return false;
			}
			wxString xPosToken = lineTokenizer->GetNextToken();
			if(!xPosToken.ToDouble(&xPos))
			{
				wxLogDebug(wxT("Invalid xPos %s"),xPosToken);
				return false;
			}

			// Get yPos
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected y Position"));
				return false;
			}
			wxString yPosToken = lineTokenizer->GetNextToken();
			if(!yPosToken.ToDouble(&yPos))
			{
				wxLogDebug(wxT("Invalid yPos"),yPosToken);
				return false;
			}

			// Get zPos
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected z Position"));
				return false;
			}
			wxString zPosToken = lineTokenizer->GetNextToken();
			if(!zPosToken.ToDouble(&zPos))
			{
				wxLogDebug(wxT("Invalid zPos"),zPosToken);
				return false;
			}
			
			// Next Line
			if(initialConditionsfileInputStream.Eof())
			{
				wxLogDebug(wxT("Expected xVel. Read in %ld"), bodiesReadCount);
				break;
			}
			line = initialConditions.ReadLine();
			lineTokenizer = new wxStringTokenizer(line);

			// Get xVel
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected x Velocity"));
				return false;
			}
			wxString xVelToken = lineTokenizer->GetNextToken();
			if(!xVelToken.ToDouble(&xVel))
			{
				wxLogDebug(wxT("Invalid xVel"),xVelToken);
				return false;
			}

			//Get yVel
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected y Velocity"));
				return false;
			}
			wxString yVelToken = lineTokenizer->GetNextToken();
			if(!yVelToken.ToDouble(&yVel))
			{
				wxLogDebug(wxT("Invalid yVel"),yVelToken);
				return false;
			}

			// Get zVel
			if(!lineTokenizer->HasMoreTokens())
			{
				wxLogDebug(wxT("Expected z Velocity"));
				return false;
			}
			wxString zVelToken = lineTokenizer->GetNextToken();
			if(!zVelToken.ToDouble(&zVel))
			{
				wxLogDebug(wxT("Invalid zVel"),zVelToken);
				return false;
			}

			this->initialPositions[i].x = xPos;
			this->initialPositions[i].y = yPos;
			this->initialPositions[i].z = zPos;
			this->initialPositions[i].w = mass * 6.67384E-08;

			this->initialVelocities[i].x = xVel;
			this->initialVelocities[i].y = yVel;
			this->initialVelocities[i].z = zVel;
			this->initialVelocities[i].w = relativisticParameter;
			
			this->physicalProperties[i].Mass = mass;
			this->physicalProperties[i].Radius = radius;
			this->physicalProperties[i].AbsoluteMagnitude = absoluteMagnitude;
			this->physicalProperties[i].RelativisticParameter = relativisticParameter;
			this->physicalProperties[i].index = i;
			
			for(size_t charIndex =0;charIndex < 32;charIndex++)
			{
				wxChar ch = 0;
				if(charIndex < nameToken.Len())
				{
					ch = nameToken.GetChar(charIndex);
				}
				this->physicalProperties[i].Name[charIndex] = ch;
			}
			this->physicalProperties[i].Name[31] =0;

			wxLogDebug(wxT("Read %ld,%s,%f,%f,%f,%f,%f,%f,%f"),bodiesReadCount++,this->physicalProperties[i].Name,mass,xPos,yPos,zPos,xVel,yVel,zVel);
		}
		else
		{
			break;
		}

		if( i % 100 == 0)
		{
			message.Printf("%d",i);
			progressBar.Update(i,message);
		}
	}
	this->initialNumParticles = bodiesReadCount;
	this->SetDefaultBodyColours();	
	
	// In slf files the moon is in Geocentic (earth centered) co-ordinates. We need it in heliocentric
	if( moon != -1 and earth != -1)
	{
		this->initialPositions[moon].x += this->initialPositions[earth].x;
		this->initialPositions[moon].y += this->initialPositions[earth].y;
		this->initialPositions[moon].z += this->initialPositions[earth].z;
		this->initialVelocities[moon].x += this->initialVelocities[earth].x;
		this->initialVelocities[moon].y += this->initialVelocities[earth].y;
		this->initialVelocities[moon].z += this->initialVelocities[earth].z;
	}

	progressBar.Close();
	return true;
}

// Create a random initial config. This will only happen if there is no initial.bin file to load
bool InitialState::CreateRandomInitialConfig()
{
	this->initialNumParticles = 8192;
	this->initialNumGrav = 16;
	this->initialJulianDate = 0;
	
	this->DeAllocate();
	if(!this->Allocate())
	{
		return false;
	}
	
	for(int i=0; i<this->initialNumParticles; i++)
	{
		wxString name = wxT("cube");
		
		this->initialColorData[i*4+0] = 255;
		this->initialColorData[i*4+1] = 255;
		this->initialColorData[i*4+2] = 255;
		this->initialColorData[i*4+3] = 255;
		
		double mass = 0.2 * double(xor128()/double(ULONG_MAX));
		this->initialPositions[i].w = mass;
		
		if(i == 0)
		{
			this->initialPositions[i].x = 0.0;
			this->initialPositions[i].y = 0.0;
			this->initialPositions[i].z = 0.0;
		}
		else
		{
			this->initialPositions[i].x = 1000.0*double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));
			this->initialPositions[i].y = 1000.0*double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));
			this->initialPositions[i].z = 1000.0*double(1.0 * (xor128()/(double(ULONG_MAX)) - 0.5));
		}
		
		this->initialVelocities[i].x=0.0;
		this->initialVelocities[i].y=0.0;
		this->initialVelocities[i].z=0.0;
		this->initialVelocities[i].w=0.0;
		
		this->physicalProperties[i].Mass = mass;
		this->physicalProperties[i].Radius = 0.0;
		this->physicalProperties[i].AbsoluteMagnitude = 0.0;
		this->physicalProperties[i].RelativisticParameter = 0.0;
		this->physicalProperties[i].index = i;
			
		for(size_t charIndex =0;charIndex < 32;charIndex++)
		{
			wxChar ch = 0;
			if(charIndex < name.Len())
			{
				ch = name.GetChar(charIndex);
			}
			this->physicalProperties[i].Name[charIndex] = ch;
		}
		this->physicalProperties[i].Name[31] =0;
	}

	return true;
}
