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

#ifndef FRAME_H_
#include "frame.hpp"
#endif // #ifndef FRAME_H_

#ifndef GLCANVAS_H_
#include "glcanvas.hpp"
#endif // #ifndef GLCANVAS_H_

#ifndef APPLICATION_H_
#define APPLICATION_H_

class Application: public wxApp
{
	public:
		virtual bool OnInit();
		virtual int OnExit();
		Application();

	private:
		bool doubleBuffer;
		bool smooth;
		bool lighting;
		char *desiredPlatform;
		int numParticles;
		int numGrav;
		bool useLastDevice;

		Frame *frame;
		bool Args(int argc, wxChar **argv);
};

#endif // #ifndef APPLICATION_H_
