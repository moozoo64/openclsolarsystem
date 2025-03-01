/*
	Copyright 2013-2025 Michael William Simmons

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
	if( this->physicalProperties != NULL )
	{
		delete[] this->physicalProperties;
		this->physicalProperties = NULL;
	}

	if( this->initialVelocities != NULL )
	{
		delete[] this->initialVelocities;
		this->initialVelocities = NULL;
	}

	if( this->initialPositions != NULL )
	{
		delete[] this->initialPositions;
		this->initialPositions = NULL;
	}

	if( this->initialColorData != NULL )
	{
		delete[] this->initialColorData;
		this->initialColorData = NULL;
	}
}

// Allocate the arrays
bool InitialState::Allocate()
{
	if( this->initialNumParticles == 0 )
	{
		wxLogError( wxT( "initialNumParticles is zero" ) );
		throw -1;
	}

	this->initialVelocities = new cl_double4[this->initialNumParticles];
	if( this->initialVelocities == NULL )
	{
		wxLogError( wxT( "Failed to Allocate initalVelocity" ) );
		throw -1;
	}

	this->initialPositions = new cl_double4[this->initialNumParticles];
	if( this->initialPositions == NULL )
	{
		wxLogError( wxT( "Failed to Allocate initalPositions" ) );
		throw -1;
	}

	unsigned int colorSize = this->initialNumParticles * 4 * sizeof( GLubyte );
	this->initialColorData = new GLubyte [colorSize];
	if( this->initialColorData == NULL )
	{
		wxLogError( wxT( "Failed to Allocate initialColorData" ) );
		throw -1;
	}

	this->physicalProperties = new PhysicalProperties[this->initialNumParticles];
	if( this->physicalProperties == NULL )
	{
		wxLogError( wxT( "Failed to Allocate physicalProperties" ) );
		throw -1;
	}

	return true;
}

/* Random Number Generator From Xorshift RNGs by George Marsaglia */
/* http://en.wikipedia.org/wiki/Xorshift */
unsigned long InitialState::xor128()
{
	static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
	unsigned long t;
	t=( x^( x<<11 ) );
	x=y;
	y=z;
	z=w;
	return( w=( w^( w>>19 ) )^( t^( t>>8 ) ) );
}

// set the colour of the bodies based on name and if its mass is being used.
void InitialState::SetDefaultBodyColours()
{
	if( this->physicalProperties == NULL || this->initialColorData == NULL )
	{
		wxLogDebug( wxT( "SetDefaultBodyColours called with physicalProperties or initialColorData NULL " ) );
		return;
	}

	for( int i=0; i<this->initialNumParticles; i++ )
	{
		wxString name = wxString( this->physicalProperties[i].Name );
		

		if( name.Contains( wxT( "InnerOort" ) ) )
		{
			this->initialColorData[i*4+0] = 192;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "OuterOort" ) ) )
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 192;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[JTrojan]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Scattered]" ) ) )
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[TNO]" ) ) )
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 92;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Centaur]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 92;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Aten]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Apollo]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 32;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Amor]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 32;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[QLess1665]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 32;
			this->initialColorData[i*4+2] = 16;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Hungaria]" ) ) )
		{
			this->initialColorData[i*4+0] = 16;
			this->initialColorData[i*4+1] = 92;
			this->initialColorData[i*4+2] = 16;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Phocaea]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 92;
			this->initialColorData[i*4+2] = 16;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Hilda]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 16;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Plutino]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[Cubewano]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[NEO!]" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "[NEO]" ) ) )
		{
			this->initialColorData[i*4+0] = 92;
			this->initialColorData[i*4+1] = 32;
			this->initialColorData[i*4+2] = 32;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "2004BL86" ) )
			|| name.Contains( wxT( "1999AN10" ) )
			|| name.Contains( wxT( "2001WN5[" ) ) 
			|| name.Contains( wxT( "1997XF11[" ) )
			|| name.Contains( wxT( "2006SU49[" ) )
			|| name.Contains( wxT( "2011FQ21[" ) )
			|| name.Contains( wxT( "Apophis" ) ))
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
    	else if( name.Contains( wxT( "Sun" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "RogueStar" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "RogueDwarf" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 192;
			this->initialColorData[i*4+2] = 192;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "RoguePlanet" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Mercury" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Venus" ) ) )
		{
			this->initialColorData[i*4+0] = 192;
			this->initialColorData[i*4+1] = 192;
			this->initialColorData[i*4+2] = 192;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Earth" ) ) )
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;

		}
		else if( name.Contains( wxT( "Moon" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;

		}
		else if( name.Contains( wxT( "Mars" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 64;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Jupiter" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Saturn" ) ) )
		{
			this->initialColorData[i*4+0] = 255;
			this->initialColorData[i*4+1] = 255;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Uranus" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Neptune" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Pluto" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Ceres" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Pallas" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Vesta" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "Juno" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
		else if( name.Contains( wxT( "C2013A1" ) ) )
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 128;
			this->initialColorData[i*4+3] = 255;
		}
		else if(i > this->initialNumGrav)
		{
			this->initialColorData[i*4+0] = 64;
			this->initialColorData[i*4+1] = 64;
			this->initialColorData[i*4+2] = 92;
			this->initialColorData[i*4+3] = 128;
		}
		else
		{
			this->initialColorData[i*4+0] = 128;
			this->initialColorData[i*4+1] = 128;
			this->initialColorData[i*4+2] = 255;
			this->initialColorData[i*4+3] = 255;
		}
	}
}

// write the initial state out to a file in binary format
bool InitialState::SaveInitialState( wxString fileName )
{
	bool success = false;
	try
	{
		wxFile stateFile;
		stateFile.ClearLastError();
		stateFile.Create( fileName,true );
		stateFile.Write( &this->initialNumParticles,sizeof( int ) );
		stateFile.Write( &this->initialNumGrav,sizeof( int ) );
		stateFile.Write( &this->initialJulianDate,sizeof( cl_double ) );
		stateFile.Write( this->initialPositions,this->initialNumParticles * sizeof( cl_double4 ) );
		stateFile.Write( this->initialVelocities,this->initialNumParticles * sizeof( cl_double4 ) );
		stateFile.Write( this->initialColorData,this->initialNumParticles * 4 * sizeof( GLubyte ) );
		stateFile.Write( this->physicalProperties,this->initialNumParticles * sizeof( PhysicalProperties ) );
		stateFile.Close();
		if( stateFile.GetLastError() != 0 )
		{
			success = false;
			wxLogError( wxT( "Save Failed" ) );
		}
		else
		{
			success = true;
		}

	}
	catch( int e )
	{
		success = false;
		wxLogError( wxT( "Save Failed" ) );
	}

	return success;
}

// loads the initial state in binary format from a file
bool InitialState::LoadInitialState( wxString fileName )
{
	wxFile stateFile;
	bool success = false;

	if( stateFile.Open( fileName ) )
	{
		stateFile.Read( &this->initialNumParticles,sizeof( int ) );
		stateFile.Read( &this->initialNumGrav,sizeof( int ) );
		stateFile.Read( &this->initialJulianDate,sizeof( cl_double ) );

		this->DeAllocate();
		if( !this->Allocate() )
		{
			wxLogError( wxT( "InitialState::LoadInitialState Allocate Failed" ) );
			throw -1;
		}

		stateFile.Read( this->initialPositions,this->initialNumParticles * sizeof( cl_double4 ) );
		stateFile.Read( this->initialVelocities,this->initialNumParticles * sizeof( cl_double4 ) );
		stateFile.Read( this->initialColorData,this->initialNumParticles * 4 * sizeof( GLubyte ) );
		stateFile.Read( this->physicalProperties,this->initialNumParticles * sizeof( PhysicalProperties ) );
		stateFile.Close();
		if( stateFile.GetLastError() != 0 )
		{
			wxLogError( wxT( "InitialState::LoadInitialState Load Failed" ) );
			success = false;
		}
		else
		{
			success = true;
		}
	}
	return success;
}

// Exports the initial state in Solex SLF file format
bool InitialState::ExportSLF( wxString fileName )
{
	wxString message;
	message.Printf( "Saving %s",fileName.c_str() );
	wxProgressDialog progressBar( message, wxT( "Saving" ), this->initialNumParticles, NULL, wxPD_AUTO_HIDE );

	// find the index for the earth
	int earth =0;
	wxString name;
	for( int i=0; i< this->initialNumParticles; i++ )
	{
		name = wxString( this->physicalProperties[i].Name );
		if( name.IsSameAs( wxT( "Earth" ) ) || name.IsSameAs( wxT( "Earth-0" ) ))
		{
			earth = i;
			break;
		}
	}

	wxFileOutputStream fileOutputStream( fileName );
	wxTextOutputStream exportSLFTextOut( fileOutputStream );
	wxString line;
	line.Printf( "%.16G\n 3 ",this->initialJulianDate );
	exportSLFTextOut.WriteString( line );
	for( int i=0; i< this->initialNumParticles; i++ )
	{
		name = wxString( this->physicalProperties[i].Name );
		double x = this->initialPositions[i].s[0];
		double y = this->initialPositions[i].s[1];
		double z = this->initialPositions[i].s[2];
		double vx = this->initialVelocities[i].s[0];
		double vy = this->initialVelocities[i].s[1];
		double vz = this->initialVelocities[i].s[2];

		if( name.IsSameAs( wxT( "Moon" ) ) || name.IsSameAs( wxT( "Moon-0" ) ))
		{
			x = x - this->initialPositions[earth].s[0];
			y = y - this->initialPositions[earth].s[1];
			z = z - this->initialPositions[earth].s[2];
			vx = vx - this->initialVelocities[earth].s[0];
			vy = vy - this->initialVelocities[earth].s[1];
			vz = vz - this->initialVelocities[earth].s[2];
		}

		line.Printf( "%.16G %.16G %.16G %.16G# %s\n",this->physicalProperties[i].Mass,this->physicalProperties[i].Radius,this->physicalProperties[i].AbsoluteMagnitude,this->physicalProperties[i].RelativisticParameter,this->physicalProperties[i].Name );
		exportSLFTextOut.WriteString( line );
		line.Printf( "%.16E %.16E %.16E\n",x,y,z );
		exportSLFTextOut.WriteString( line );
		line.Printf( "%.16E %.16E %.16E\n",vx,vy,vz );
		exportSLFTextOut.WriteString( line );

		if( i % 100 == 0 )
		{
			message.Printf( "%d",i );
			progressBar.Update( i,message );
		}
	}
	return true;
}

// imports a Solex SLF formated file
bool InitialState::ImportSLF( wxString fileName )
{
	this->initialNumParticles = 2000000;
	this->initialNumGrav = 16;

	wxString message;
	message.Printf( "Loading %s",fileName.c_str() );
	wxProgressDialog progressBar( message, wxT( "Loading" ), this->initialNumParticles, NULL, wxPD_AUTO_HIDE );

	this->DeAllocate();
	if( !this->Allocate() )
	{
		return false;
	}

	wxFileInputStream initialConditionsfileInputStream( fileName );
	wxTextInputStream initialConditions( initialConditionsfileInputStream );

	if( initialConditionsfileInputStream.Eof() )
	{
		wxLogDebug( wxT( "File is Empty" ) );
		return false;
	}

	progressBar.Update( 0,wxT( "Loading" ) );

	int bodiesReadCount = 0;
	wxString FirstLine = initialConditions.ReadLine();
	double time;
	FirstLine.ToDouble( &time );
	this->initialJulianDate = time;

	this->SetDefaultBodyColours();

	int moon = -1;
	int earth = -1;
	for( int i=0; i< this->initialNumParticles; i++ )
	{
		this->initialPositions[i].s[3] = 0.2 * double( xor128()/double( ULONG_MAX ) );
		this->initialPositions[i].s[0] = double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );
		this->initialPositions[i].s[1] = double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );
		this->initialPositions[i].s[2] = double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );

		this->initialVelocities[i].s[0]=0.0;
		this->initialVelocities[i].s[1]=0.0;
		this->initialVelocities[i].s[2]=0.0;
		this->initialVelocities[i].s[3]=0.0;

		if( !initialConditionsfileInputStream.Eof() )
		{
			double mass,radius,absoluteMagnitude,relativisticParameter,xPos,yPos,zPos,xVel,yVel,zVel;

			if( initialConditionsfileInputStream.Eof() )
			{
				wxLogDebug( wxT( "Read in %ld" ), bodiesReadCount );
				break;
			}
			wxString line = initialConditions.ReadLine();
			wxStringTokenizer *lineTokenizer = new wxStringTokenizer( line );
			if( i == 0 )
			{
				lineTokenizer->GetNextToken();
			}

			// Get Mass
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected mass" ) );
				break;
			}
			wxString massToken = lineTokenizer->GetNextToken();
			if( !massToken.ToDouble( &mass ) )
			{
				wxLogDebug( wxT( "Invalid mass %s" ), massToken );
				return false;
			}

			// Get Radius
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected radius" ) );
				break;
			}
			wxString radiusToken = lineTokenizer->GetNextToken();
			if( !radiusToken.ToDouble( &radius ) )
			{
				wxLogDebug( wxT( "Invalid radius %s" ), radiusToken );
				return false;
			}

			// Get absolute magnitude
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected absolute magnitude" ) );
				break;
			}
			wxString absoluteMagnitudeToken = lineTokenizer->GetNextToken();
			if( !absoluteMagnitudeToken.ToDouble( &absoluteMagnitude ) )
			{
				wxLogDebug( wxT( "Invalid absolute magnitude %s" ), absoluteMagnitudeToken );
				return false;
			}

			// Get relativistic parameter
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected relativistic parameter" ) );
				break;
			}
			wxString relativisticParameterToken = lineTokenizer->GetNextToken();
			if( relativisticParameterToken.EndsWith( wxT( "#" ),NULL ) )
			{
				relativisticParameterToken.RemoveLast();
			}
			if( !relativisticParameterToken.ToDouble( &relativisticParameter ) )
			{
				wxLogDebug( wxT( "Invalid relativisticParameter" ),relativisticParameterToken );
				return false;
			}
			// Get name
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected name" ) );
				break;
			}
			wxString nameToken = lineTokenizer->GetNextToken();

			if( nameToken.IsSameAs( wxT( "Earth" ) ) || nameToken.IsSameAs( wxT( "Earth-0" ) ))
			{
				earth = i;
                wxLogDebug( wxT( "Found Earth" ) );
			}
			else if( nameToken.IsSameAs( wxT( "Moon" ) ) || nameToken.IsSameAs( wxT( "Moon-0" ) ))
			{
				moon = i;
                wxLogDebug( wxT( "Found Moon" ) );
			}

			// Next Line
			if( initialConditionsfileInputStream.Eof() )
			{
				wxLogDebug( wxT( "Expected xPos. Read in %ld" ), bodiesReadCount );
				break;
			}
			line = initialConditions.ReadLine();
			lineTokenizer = new wxStringTokenizer( line );

			// Get xPos
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected x Position" ) );
				return false;
			}
			wxString xPosToken = lineTokenizer->GetNextToken();
			if( !xPosToken.ToDouble( &xPos ) )
			{
				wxLogDebug( wxT( "Invalid xPos %s" ),xPosToken );
				return false;
			}

			// Get yPos
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected y Position" ) );
				return false;
			}
			wxString yPosToken = lineTokenizer->GetNextToken();
			if( !yPosToken.ToDouble( &yPos ) )
			{
				wxLogDebug( wxT( "Invalid yPos" ),yPosToken );
				return false;
			}

			// Get zPos
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected z Position" ) );
				return false;
			}
			wxString zPosToken = lineTokenizer->GetNextToken();
			if( !zPosToken.ToDouble( &zPos ) )
			{
				wxLogDebug( wxT( "Invalid zPos" ),zPosToken );
				return false;
			}

			// Next Line
			if( initialConditionsfileInputStream.Eof() )
			{
				wxLogDebug( wxT( "Expected xVel. Read in %ld" ), bodiesReadCount );
				break;
			}
			line = initialConditions.ReadLine();
			lineTokenizer = new wxStringTokenizer( line );

			// Get xVel
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected x Velocity" ) );
				return false;
			}
			wxString xVelToken = lineTokenizer->GetNextToken();
			if( !xVelToken.ToDouble( &xVel ) )
			{
				wxLogDebug( wxT( "Invalid xVel" ),xVelToken );
				return false;
			}

			//Get yVel
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected y Velocity" ) );
				return false;
			}
			wxString yVelToken = lineTokenizer->GetNextToken();
			if( !yVelToken.ToDouble( &yVel ) )
			{
				wxLogDebug( wxT( "Invalid yVel" ),yVelToken );
				return false;
			}

			// Get zVel
			if( !lineTokenizer->HasMoreTokens() )
			{
				wxLogDebug( wxT( "Expected z Velocity" ) );
				return false;
			}
			wxString zVelToken = lineTokenizer->GetNextToken();
			if( !zVelToken.ToDouble( &zVel ) )
			{
				wxLogDebug( wxT( "Invalid zVel" ),zVelToken );
				return false;
			}

			this->initialPositions[i].s[0] = xPos;
			this->initialPositions[i].s[1] = yPos;
			this->initialPositions[i].s[2] = zPos;
			//this->initialPositions[i].w = mass * 6.67384E-08;

			if( mass < 1.0E-30f )
			{
				this->initialPositions[i].s[3] = 2.83E-09 * 6.67384E-08;
			}
			else
			{
				this->initialPositions[i].s[3] = mass * 6.67384E-08;
			}

			this->initialVelocities[i].s[0] = xVel;
			this->initialVelocities[i].s[1] = yVel;
			this->initialVelocities[i].s[2] = zVel;
			this->initialVelocities[i].s[3] = relativisticParameter;

			this->physicalProperties[i].Mass = mass;
			this->physicalProperties[i].Radius = radius;
			this->physicalProperties[i].AbsoluteMagnitude = absoluteMagnitude;
			this->physicalProperties[i].RelativisticParameter = relativisticParameter;
			this->physicalProperties[i].Index = i;

			for( size_t charIndex =0; charIndex < 32; charIndex++ )
			{
				wxChar ch = 0;
				if( charIndex < nameToken.Len() )
				{
					ch = nameToken.GetChar( charIndex );
				}
				this->physicalProperties[i].Name[charIndex] = ch;
			}
			this->physicalProperties[i].Name[31] =0;

			wxLogDebug( wxT( "Read %ld,%s,%e,%e,%f,%f,%f,%f,%f,%f" ),bodiesReadCount,this->physicalProperties[i].Name,mass,this->initialPositions[i].s[3],xPos,yPos,zPos,xVel,yVel,zVel );
			bodiesReadCount++;
		}
		else
		{
			break;
		}

		if( i % 100 == 0 )
		{
			message.Printf( "%d",i );
			progressBar.Update( i,message );
		}
	}

	//wxLogMessage( wxT( "Read %ld Bodies" ),bodiesReadCount);
	this->initialNumParticles = bodiesReadCount;
	this->SetDefaultBodyColours();

	// In slf files the moon is in Geocentic (earth centered) co-ordinates. We need it in heliocentric
	if( moon != -1 and earth != -1 )
	{
		this->initialPositions[moon].s[0] += this->initialPositions[earth].s[0];
		this->initialPositions[moon].s[1] += this->initialPositions[earth].s[1];
		this->initialPositions[moon].s[2] += this->initialPositions[earth].s[2];
		this->initialVelocities[moon].s[0] += this->initialVelocities[earth].s[0];
		this->initialVelocities[moon].s[1] += this->initialVelocities[earth].s[1];
		this->initialVelocities[moon].s[2] += this->initialVelocities[earth].s[2];
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
	if( !this->Allocate() )
	{
		return false;
	}

	for( int i=0; i<this->initialNumParticles; i++ )
	{
		wxString name = wxT( "cube" );

		this->initialColorData[i*4+0] = 255;
		this->initialColorData[i*4+1] = 255;
		this->initialColorData[i*4+2] = 255;
		this->initialColorData[i*4+3] = 255;

		double mass = 0.2 * double( xor128()/double( ULONG_MAX ) );
		this->initialPositions[i].s[3] = mass;

		if( i == 0 )
		{
			this->initialPositions[i].s[0] = 0.0;
			this->initialPositions[i].s[1] = 0.0;
			this->initialPositions[i].s[2] = 0.0;
		}
		else
		{
			this->initialPositions[i].s[0] = 1000.0*double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );
			this->initialPositions[i].s[1] = 1000.0*double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );
			this->initialPositions[i].s[2] = 1000.0*double( 1.0 * ( xor128()/( double( ULONG_MAX ) ) - 0.5 ) );
		}

		this->initialVelocities[i].s[0]=0.0;
		this->initialVelocities[i].s[1]=0.0;
		this->initialVelocities[i].s[2]=0.0;
		this->initialVelocities[i].s[3]=0.0;

		this->physicalProperties[i].Mass = mass;
		this->physicalProperties[i].Radius = 0.0;
		this->physicalProperties[i].AbsoluteMagnitude = 0.0;
		this->physicalProperties[i].RelativisticParameter = 0.0;
		this->physicalProperties[i].Index = i;

		for( size_t charIndex =0; charIndex < 32; charIndex++ )
		{
			wxChar ch = 0;
			if( charIndex < name.Len() )
			{
				ch = name.GetChar( charIndex );
			}
			this->physicalProperties[i].Name[charIndex] = ch;
		}
		this->physicalProperties[i].Name[31] =0;
	}

	return true;
}
