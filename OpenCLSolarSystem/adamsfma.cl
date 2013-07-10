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

#define KMTOGM 1.0/1000000

__kernel
void newtonian( 
__constant double4* gravPos,
__global double4* pos, 
int numGrav, 
double epsSqr, 
__global double4* acc) 
{ 
	unsigned int gid = get_global_id(0); 
	double4 myPos = pos[gid]; 
	double4 newAcc = (double4)(0.0f, 0.0f, 0.0f, 0.0f);
	double4 r;
	double distSqr;
	double invDist;
	double invDistCube;
	double s;
	
	// Do the Sun
	r = gravPos[0] - myPos;
	r.w =0.0;
	distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
	invDist = rsqrt(distSqr + epsSqr); 
	invDistCube = invDist * invDist * invDist; 
	s = gravPos[0].w * invDistCube;
	double4 accSun= s * r; 
	
	//Do the rest
	for(int gravBody = 1; gravBody < numGrav; gravBody++)
	{
		r = gravPos[gravBody] - myPos;
		r.w =0.0;
		distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
		invDist = rsqrt(distSqr + epsSqr); 
		invDistCube = invDist * invDist * invDist; 
		s = gravPos[gravBody].w * invDistCube; 
		newAcc += s * r; 
	}
	
	acc[gid] = newAcc + accSun;
}

#define relativisticC1 8.86221439924785E-03

__kernel
void relativistic( 
__constant double4* gravPos,
__global double4* pos,
__global double4* vel,
int numGrav, 
double epsSqr, 
__global double4* acc) 
{ 
	unsigned int gid = get_global_id(0); 
	double4 myPos = pos[gid];
	double4 myVel = vel[gid];
	double4 newAcc = (double4)(0.0f, 0.0f, 0.0f, 0.0f);
	double4 r;
	double distSqr;
	double invDist;
	double invDistCube;
	double s;
	
	// Do the Sun
	r = gravPos[0] - myPos;
	r.w =0.0;
	distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
	invDist = rsqrt(distSqr + epsSqr); 
	invDistCube = invDist * invDist * invDist; 
	s = gravPos[0].w * invDistCube;
	s = s * (1.0 + myVel.w + (relativisticC1*invDist));
	double4 accSun= s * r;

    //Do the rest
	for(int gravBody = 1; gravBody < numGrav; gravBody++)
	{
		r = gravPos[gravBody] - myPos;
		r.w =0.0;
		distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
		invDist = rsqrt(distSqr + epsSqr); 
		invDistCube = invDist * invDist * invDist; 
		s = gravPos[gravBody].w * invDistCube; 
		newAcc += s * r; 
	}
	
	acc[gid] = newAcc + accSun;
}

__kernel
void relativisticLocal( 
__constant double4* gravPos,
__global double4* pos,
__global double4* vel,
int numGrav, 
double epsSqr, 
__global double4* acc,
__local double4* localGravPos) 
{ 
	uint gid = get_global_id(0);
	uint lid = get_local_id(0); 
	double4 myPos = pos[gid];
	double4 myVel = vel[gid];

	double4 r;
	double distSqr;
	double invDist;
	double invDistCube;
	double s;
	
	uint blockSize = get_local_size(0);
	uint numBlocks = numGrav/blockSize;
	double4 newAcc = (double4)(0.0f, 0.0f, 0.0f, 0.0f);
	double4 accSun = (double4)(0.0f, 0.0f, 0.0f, 0.0f);
    double4 gravPosN;
	for(uint block = 0; block < numBlocks; block++)
	{
		localGravPos[lid] = gravPos[block*numBlocks+lid];
		barrier(CLK_LOCAL_MEM_FENCE);
		uint start =0;
		if(block == 0)
		{
			// Do the Sun
			gravPosN =localGravPos[0];
			r = gravPosN - myPos;
			r.w =0.0;
			distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
			invDist = rsqrt(distSqr + epsSqr); 
			invDistCube = invDist * invDist * invDist; 
			s = gravPosN.w * invDistCube;
			s = s * (1.0 + myVel.w + (relativisticC1*invDist));
			accSun= s * r;
			start = 1;
		}
		for( uint gravBody = start ;gravBody < blockSize ; gravBody++)
		{
			//Do the rest
           gravPosN =localGravPos[gravBody];
			r = gravPosN - myPos;
			r.w =0.0;
			distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
			invDist = rsqrt(distSqr + epsSqr); 
			invDistCube = invDist * invDist * invDist; 
			s = gravPosN.w * invDistCube; 
			newAcc += s * r;
		}
	    barrier(CLK_LOCAL_MEM_FENCE);
	}
	acc[gid] = newAcc + accSun;
}

__kernel
void copyToDisplay(
__constant double4* gravPos,
__global double4* pos,
__global float4* dispPos,
int centerBodyIndex) 
{ 
	double4 dispPosDouble;
	float4 dispPosFloat;
	unsigned int gid = get_global_id(0);
	dispPosDouble = pos[gid] - gravPos[centerBodyIndex];
	
	dispPosFloat.x = (float) dispPosDouble.x;
	dispPosFloat.y = (float) dispPosDouble.y;
	dispPosFloat.z = (float) dispPosDouble.z;
	dispPosFloat.w = (float) dispPosDouble.w;
	
	dispPos[gid] = dispPosFloat;
}

#define B2C1 1.500000000000000000
#define B2C2 -0.50000000000000000
#define M2C1 0.500000000000000000
#define M2C2 0.500000000000000000

#define B4C1  2.291666666666666666666666666666666666
#define B4C2 -2.458333333333333333333333333333333333
#define B4C3  1.541666666666666666666666666666666666
#define B4C4 -0.375000000000000000000000000000000000

#define M4C1  0.375000000000000000000000000000000000
#define M4C2  0.791666666666666666666666666666666666
#define M4C3 -0.208333333333333333333333333333333333
#define M4C4  0.041666666666666666666666666666666666

#define B8C1   3.589955357142857142857142857142857142
#define B8C2  -9.525206679894179894179894179894179894
#define B8C3  18.054538690476190476190476190476190476
#define B8C4 -22.027752976190476190476190476190476190
#define B8C5  17.379654431216931216931216931216931216
#define B8C6  -8.612127976190476190476190476190476190
#define B8C7   2.445163690476190476190476190476190476
#define B8C8  -0.304224537037037037037037037037037037

#define M8C1   0.304224537037037037037037037037037037
#define M8C2   1.156159060846560846560846560846560846
#define M8C3  -1.006919642857142857142857142857142857
#define M8C4   1.017964616402116402116402116402116402
#define M8C5  -0.732035383597883597883597883597883597
#define M8C6   0.343080357142857142857142857142857142
#define M8C7  -0.093840939153439153439153439153439153
#define M8C8   0.011367394179894179894179894179894179

#define B10C1   4.171798804012345679012345679012345679
#define B10C2 -14.466929701278659611992945326278659611
#define B10C3  36.641958774250440917107583774250440917
#define B10C4 -62.646298500881834215167548500881834215
#define B10C5  74.179320712081128747795414462081128747
#define B10C6 -61.283642250881834215167548500881834215
#define B10C7  34.807405202821869488536155202821869488
#define B10C8 -12.994284611992945326278659611992945326
#define B10C9   2.877647018298059964726631393298059964
#define B10C10 -0.286975446428571428571428571428571428

#define M10C1   0.286975446428571428571428571428571428
#define M10C2   1.302044339726631393298059964726631393
#define M10C3  -1.553034611992945326278659611992945326
#define M10C4   2.204905202821869488536155202821869488
#define M10C5  -2.381454750881834215167548500881834215
#define M10C6   1.861508212081128747795414462081128747
#define M10C7  -1.018798500881834215167548500881834215
#define M10C8   0.370351631393298059964726631393298059
#define M10C9  -0.080389522707231040564373897707231040
#define M10C10  0.007892554012345679012345679012345679

#define B11C1    4.451988400456282400726845171289615734
#define B11C2  -17.268825665718026829137940249051360162
#define B11C3   49.250490614227593394260060926727593394
#define B11C4  -96.269050074154240820907487574154240820
#define B11C5  133.019135965307840307840307840307840307
#define B11C6 -131.891420554753888087221420554753888087
#define B11C7   93.647220456048581048581048581048581048
#define B11C8  -46.617036185265351932018598685265351932
#define B11C9   15.486178858275212441879108545775212441
#define B11C10   -3.088871410867938645716423494201271979
#define B11C11    0.280189596443936721714499492277270055

#define M11C1    0.280189596443936721714499492277270055
#define M11C2    1.369902839572978461867350756239645128
#define M11C3   -1.858397861301507134840468173801507134
#define M11C4    3.019207200978034311367644700978034311
#define M11C5   -3.806483247655122655122655122655122655
#define M11C6    3.571542408209074875741542408209074875
#define M11C7   -2.443826997655122655122655122655122655
#define M11C8    1.184653629549462882796216129549462882
#define M11C9   -0.385752772015792849126182459515792849
#define M11C10    0.075751053858692747581636470525359414
#define M11C11   -0.006785849984634706856929079151301373

#define B12C1    4.726253940487881460103682325904548126
#define B12C2  -20.285746606065616482283148949815616482
#define B12C3   64.335095315965541659986104430548874993
#define B12C4 -141.522864179368085618085618085618085618
#define B12C5  223.526764175735529902196568863235529902
#define B12C6 -258.602100049352653519320185986852653519
#define B12C7  220.357899950647346480679814013147346480
#define B12C8 -137.124664395693041526374859708193041526
#define B12C9   60.739992963489057239057239057239057239
#define B12C10  -18.173476112605886911442466998022553578
#define B12C11    3.297110536791526374859708193041526374
#define B12C12   -0.274265540031599059376837154614932392

#define M12C1    0.274265540031599059376837154614932392
#define M12C2    1.435067460108692747581636470525359414
#define M12C3   -2.184220963980078563411896745230078563
#define M12C4    3.996676509013748597081930415263748597
#define M12C5   -5.761421863726551226551226551226551226
#define M12C6    6.308456470709074875741542408209074875
#define M12C7   -5.180741060155122655122655122655122655
#define M12C8    3.139592245620891454224787558120891454
#define M12C9   -1.363222080051507134840468173801507134
#define M12C10    0.401574156537264176153065041953930842
#define M12C11   -0.071950470520348992571214793437015659
#define M12C12    0.005924056412337662337662337662337662

#define B16C1     5.776080028330126933659781984296975478
#define B16C2   -34.437212290517538206168056256239677756
#define B16C3   153.295313563544223903709132986028929591
#define B16C4  -487.624165234214119606673619019298031643
#define B16C5  1155.966252758585158844886578572645592045
#define B16C6 -2102.383615302252961827063458456756516721
#define B16C7  2986.586096092599207006862430142853423276
#define B16C8 -3345.962146803640596785024695077605130515
#define B16C9  2964.670665491647888979651545789111926678
#define B16C10 -2069.993131887207256044750533286688489510
#define B16C11 1126.751450793321070254905131448341324884
#define B16C12 -468.674127888398662742109611598147753350
#define B16C13  143.998829364498292156884529018567819273
#define B16C14  -30.818758034463882040904748135788700162
#define B16C15    4.104778844743438324649744402830822583
#define B16C16   -0.256309496574389152514152514152514152

#define M16C1      0.256309496574389152514152514152514152
#define M16C2      1.675128083139900493433341757856749038
#define M16C3     -3.680072701590839904469754557937979454
#define M16C4      9.761995481886298495783725060621004183
#define M16C5    -21.140881468825862030916043261722274067
#define M16C6     36.406371721653340663068396754463773864
#define M16C7    -49.857166734544628493730125123423183387
#define M16C8     54.405455281587302244957668238091518514
#define M16C9    -47.258925891252203927881837934747987658
#define M16C10    32.490024680635984217746783884350021916
#define M16C11   -17.466683319498922711417199953355156177
#define M16C12     7.191569756389252073086949630159506702
#define M16C13    -2.190844123010405166352035840571995774
#define M16C14     0.465511282840366748959121093159893865
#define M16C15    -0.061618445537183739206446437487001860
#define M16C16     0.003826899553211884423304176390596143

__kernel
void adamsStartup( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	int order = step > 1? (step > 16? 4 : 2) : 1;
	
	if (stage == 1)
	{
		// Adams-Bashford Predictor
		// acceleration
		if(order == 1)
		{
			f = acceleration;
			sum = 1.0 * f;
		}
		else if (order == 2)
		{
			f = acceleration;
			sum = B2C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(B2C2,f,sum); //sum += B2C2 * f;
		}
		else
		{
			f = acceleration;
			sum = B4C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(B4C2,f,sum); //sum += B4C2 * f;
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(B4C3,f,sum); //sum += B4C3 * f;

			index = ((step-3) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(B4C4,f,sum); //sum += B4C4 * f;
		}
		
		newVelocity = velocity + deltaTime * sum;
		velLast[gid] = velocity;

		// --------------------------------------------
		// position
		if(order == 1)
		{
			f = velocity;
			sum = 1.0 * f;
		}
		else if (order == 2)
		{
			f = velocity;
			sum = B2C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(B2C2,f,sum); //sum += B2C2 * f;
		}
		else
		{
			f = velocity;
			sum = B4C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(B4C2,f,sum); //sum += B4C2 * f;
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(B4C3,f,sum); //sum += B4C3 * f;
			
			index = ((step-3) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(B4C4,f,sum); //sum += B4C4 * f;
		}
		
		newPosition = position + deltaTime * sum * (KMTOGM);
		posLast[gid] = position;
		
		index = ((step) & 0xF) * numParticles + gid;
		//velocity.w = (double) step;
		velHistory[index] = velocity;
		//acceleration.w = (double)step;
		accHistory[index] = acceleration;
	}
	else
	{
		// Adams-Moulton corrector
		// acceleration -> velocity
		
		if(order == 1)
		{
			f = acceleration;
			sum = 1.0 * f;
		}
		else if (order == 2)
		{
			f = acceleration;
			sum = M2C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(M2C2,f,sum); //sum += M2C2 * f;
		}
		else
		{
			f = acceleration;
			sum = M4C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(M4C2,f,sum); //sum += M4C2 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(M4C3,f,sum); //sum += M4C3 * f;
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum = fma(M4C4,f,sum); //sum += M4C4 * f;
		}
		
		// Store corrected velocity
		newVelocity = velLast[gid] + deltaTime * sum;
		
		//------------------------------
		// velocity -> position
		if(order == 1)
		{
			f = velocity;
			sum = 1.0 * f;
		}
		else if (order == 2)
		{
			f = velocity;
			sum = M2C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(M2C2,f,sum); //sum += M2C2 * f;
		}
		else
		{		
			f = velocity;
			sum = M4C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(M4C2,f,sum); //sum += M4C2 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(M4C3,f,sum); //sum += M4C3 * f;
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum = fma(M4C4,f,sum); //sum += M4C4 * f;
		}
		
		// Store corrected position
		newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	}
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth12( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B12C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C2,f,sum); //sum += B12C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C3,f,sum); //sum += B12C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C4,f,sum); //sum += B12C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C5,f,sum); //sum += B12C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C6,f,sum); //sum += B12C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C7,f,sum); //sum += B12C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C8,f,sum); //sum += B12C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C9,f,sum); //sum += B12C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C10,f,sum); //sum += B12C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C11,f,sum); //sum += B12C11 * f;
	
	index = ((step-11) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B12C12,f,sum); //sum += B12C12 * f;
	
	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position
	f = velocity;
	sum = B12C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C2,f,sum); //sum += B12C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C3,f,sum); //sum += B12C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C4,f,sum); //sum += B12C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C5,f,sum); //sum += B12C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C6,f,sum); //sum += B12C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C7,f,sum); //sum += B12C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C8,f,sum); //sum += B12C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C9,f,sum); //sum += B12C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C10,f,sum); //sum += B12C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C11,f,sum); //sum += B12C11 * f;
	
	index = ((step-11) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B12C12,f,sum); //sum += B12C12 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}


__kernel
void adamsMoulton11( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Moulton corrector
	// acceleration -> velocity
	
	f = acceleration;
	sum = M12C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C2,f,sum); //sum += M12C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C3,f,sum); //sum += M12C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C4,f,sum); //sum += M12C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C5,f,sum); //sum += M12C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C6,f,sum); //sum += M12C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C7,f,sum); //sum += M12C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C8,f,sum); //sum += M12C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C9,f,sum); //sum += M12C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C10,f,sum); //sum += M12C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C11,f,sum); //sum += M12C11 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M12C12,f,sum); //sum += M12C12 * f;
	
	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M12C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C2,f,sum); //sum += M12C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C3,f,sum); //sum += M12C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C4,f,sum); //sum += M12C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C5,f,sum); //sum += M12C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C6,f,sum); //sum += M12C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C7,f,sum); //sum += M12C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C8,f,sum); //sum += M12C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C9,f,sum); //sum += M12C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C10,f,sum); //sum += M12C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C11,f,sum); //sum += M12C11 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M12C12,f,sum); //sum += M12C12 * f;
	
	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth11( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B11C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C2,f,sum); //sum += B11C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C3,f,sum); //sum += B11C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C4,f,sum); //sum += B11C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C5,f,sum); //sum += B11C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C6,f,sum); //sum += B11C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C7,f,sum); //sum += B11C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C8,f,sum); //sum += B11C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C9,f,sum); //sum += B11C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C10,f,sum); //sum += B11C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B11C11,f,sum); //sum += B11C11 * f;
	
	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position
	f = velocity;
	sum = B11C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C2,f,sum); //sum += B11C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C3,f,sum); //sum += B11C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C4,f,sum); //sum += B11C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C5,f,sum); //sum += B11C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C6,f,sum); //sum += B11C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C7,f,sum); //sum += B11C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C8,f,sum); //sum += B11C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C9,f,sum); //sum += B11C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C10,f,sum); //sum += B11C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B11C11,f,sum); //sum += B11C11 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsMoulton10( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Moulton corrector
	// acceleration -> velocity
	
	f = acceleration;
	sum = M11C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C2,f,sum); //sum += M11C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C3,f,sum); //sum += M11C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C4,f,sum); //sum += M11C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C5,f,sum); //sum += M11C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C6,f,sum); //sum += M11C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C7,f,sum); //sum += M11C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C8,f,sum); //sum += M11C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C9,f,sum); //sum += M11C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C10,f,sum); //sum += M11C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M11C11,f,sum); //sum += M11C11 * f;
	
	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M11C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C2,f,sum); //sum += M11C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C3,f,sum); //sum += M11C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C4,f,sum); //sum += M11C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C5,f,sum); //sum += M11C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C6,f,sum); //sum += M11C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C7,f,sum); //sum += M11C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C8,f,sum); //sum += M11C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C9,f,sum); //sum += M11C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C10,f,sum); //sum += M11C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M11C11,f,sum); //sum += M11C11 * f;
	
	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth10( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B10C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C2,f,sum); //sum += B10C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C3,f,sum); //sum += B10C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C4,f,sum); //sum += B10C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C5,f,sum); //sum += B10C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C6,f,sum); //sum += B10C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C7,f,sum); //sum += B10C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C8,f,sum); //sum += B10C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C9,f,sum); //sum += B10C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B10C10,f,sum); //sum += B10C10 * f;
	
	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position
	f = velocity;
	sum = B10C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C2,f,sum); //sum += B10C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C3,f,sum); //sum += B10C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C4,f,sum); //sum += B10C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C5,f,sum); //sum += B10C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C6,f,sum); //sum += B10C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C7,f,sum); //sum += B10C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C8,f,sum); //sum += B10C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C9,f,sum);  //sum += B10C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B10C10,f,sum); //sum += B10C10 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsMoulton9( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Moulton corrector
	// acceleration -> velocity
	
	f = acceleration;
	sum = M10C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C2,f,sum); //sum += M10C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C3,f,sum); //sum += M10C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C4,f,sum); //sum += M10C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C5,f,sum); //sum += M10C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C6,f,sum); //sum += M10C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C7,f,sum); //sum += M10C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C8,f,sum); //sum += M10C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C9,f,sum); //sum += M10C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M10C10,f,sum); //sum += M10C10 * f;
	
	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M10C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C2,f,sum); //sum += M10C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C3,f,sum); //sum += M10C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C4,f,sum); //sum += M10C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C5,f,sum); //sum += M10C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C6,f,sum); //sum += M10C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C7,f,sum); //sum += M10C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C8,f,sum); //sum += M10C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C9,f,sum); //sum += M10C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M10C10,f,sum); //sum += M10C10 * f;
	
	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth8( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B8C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C2,f,sum); //sum += B8C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C3,f,sum); //sum += B8C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C4,f,sum); //sum += B8C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C5,f,sum); //sum += B8C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C6,f,sum); //sum += B8C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C7,f,sum); //sum += B8C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B8C8,f,sum); //sum += B8C8 * f;
	
	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position
	f = velocity;
	sum = B8C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C2,f,sum); //sum += B8C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C3,f,sum); //sum += B8C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C4,f,sum); //sum += B8C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C5,f,sum); //sum += B8C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C6,f,sum); //sum += B8C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C7,f,sum); //sum += B8C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B8C8,f,sum); //sum += B8C8 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;

	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}
__kernel void adamsMoulton7( 
	__global double4* pos, 
	__global double4* vel,
	__global double4* acc, 
	double deltaTime, 
	__global double4* newPos, 
	__global double4* newVel,
	int stage,
	int step,
	int numParticles,
	__global double4* posLast,
	__global double4* velLast,
	__global double4* velHistory,
	__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
		
	// Adams-Moulton corrector
	// acceleration -> velocity
	
	f = acceleration;
	sum = M8C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C2,f,sum); //sum += M8C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C3,f,sum); //sum += M8C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C4,f,sum); //sum += M8C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C5,f,sum); //sum += M8C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C6,f,sum); //sum += M8C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C7,f,sum); //sum += M8C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M8C8,f,sum); //sum += M8C8 * f;
	
	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M8C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C2,f,sum); //sum += M8C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C3,f,sum); //sum += M8C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C4,f,sum); //sum += M8C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C5,f,sum); //sum += M8C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C6,f,sum); //sum += M8C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C7,f,sum); //sum += M8C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M8C8,f,sum); //sum += M8C8 * f;
	
	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);

	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth4( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B4C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B4C2,f,sum); //sum += B4C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B4C3,f,sum); //sum += B4C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B4C4,f,sum); //sum += B4C4 * f;
	
	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position

	f = velocity;
	sum = B4C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B4C2,f,sum); //sum += B4C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B4C3,f,sum); //sum += B4C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B4C4,f,sum); //sum += B4C4 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsMoulton3( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Moulton corrector
	// acceleration -> velocity
	f = acceleration;
	sum = M4C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M4C2,f,sum); //sum += M4C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M4C3,f,sum); //sum += M4C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M4C4,f,sum); //sum += M4C4 * f;
	
	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M4C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M4C2,f,sum); //sum += M4C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M4C3,f,sum); //sum += M4C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M4C4,f,sum); //sum += M4C4 * f;
	
	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}

__kernel
void adamsBashforth16( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Bashford Predictor
	// acceleration
	f = acceleration;
	sum = B16C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C2,f,sum); //sum += B16C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C3,f,sum); //sum += B16C3 * f;

	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C4,f,sum); //sum += B16C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C5,f,sum); //sum += B16C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C6,f,sum); //sum += B16C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C7,f,sum); //sum += B16C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C8,f,sum); //sum += B16C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C9,f,sum); //sum += B16C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C10,f,sum); //sum += B16C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C11,f,sum); //sum += B16C11 * f;
	
	index = ((step-11) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C12,f,sum); //sum += B16C12 * f;

	index = ((step-12) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C13,f,sum); //sum += B16C13 * f;
	
	index = ((step-13) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C14,f,sum); //sum += B16C14 * f;
	
	index = ((step-14) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C15,f,sum); //sum += B16C15 * f;

    index = ((step-15) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(B16C16,f,sum); //sum += B16C16 * f;

	newVelocity = velocity + deltaTime * sum;
	velLast[gid] = velocity;

	// --------------------------------------------
	// position
	f = velocity;
	sum = B16C1 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C2,f,sum); //sum += B16C2 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C3,f,sum); //sum += B16C3 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C4,f,sum); //sum += B16C4 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C5,f,sum); //sum += B16C5 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C6,f,sum); //sum += B16C6 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C7,f,sum); //sum += B16C7 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C8,f,sum); //sum += B16C8 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C9,f,sum); //sum += B16C9 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C10,f,sum); //sum += B16C10 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C11,f,sum); //sum += B16C11 * f;
	
	index = ((step-11) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C12,f,sum); //sum += B16C12 * f;
	
	index = ((step-12) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C13,f,sum); //sum += B16C13 * f;
	
	index = ((step-13) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C14,f,sum); //sum += B16C14 * f;
	
	index = ((step-14) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C15,f,sum); //sum += B16C15 * f;

	index = ((step-15) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(B16C16,f,sum); //sum += B16C16 * f;
	
	newPosition = position + deltaTime * sum * (KMTOGM);
	posLast[gid] = position;
	
	index = ((step) & 0xF) * numParticles + gid;
	//velocity.w = (double) step;
	velHistory[index] = velocity;
	//acceleration.w = (double)step;
	accHistory[index] = acceleration;
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}


__kernel
void adamsMoulton15( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPos, 
__global double4* newVel,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position = pos[gid]; 
	double4 velocity = vel[gid];
	double4 acceleration = acc[gid];
	long index;
	double4 newPosition;
	double4 newVelocity;
	double4 sum;
	double4 f;
	
	// Adams-Moulton corrector
	// acceleration -> velocity
	
	f = acceleration;
	sum = M16C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C2,f,sum); //sum += M16C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C3,f,sum); //sum += M16C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C4,f,sum); //sum += M16C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C5,f,sum); //sum += M16C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C6,f,sum); //sum += M16C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C7,f,sum); //sum += M16C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C8,f,sum); //sum += M16C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C9,f,sum); //sum += M16C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C10,f,sum); //sum += M16C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C11,f,sum); //sum += M16C11 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C12,f,sum); //sum += M16C12 * f;

	index = ((step-11) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C13,f,sum); //sum += M16C13 * f;
	
	index = ((step-12) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C14,f,sum); //sum += M16C14 * f;
	
	index = ((step-13) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C15,f,sum); //sum += M16C15 * f;
	
	index = ((step-14) & 0xF) * numParticles + gid;
	f = accHistory[index];
	sum = fma(M16C16,f,sum); //sum += M16C16 * f;

	// Store corrected velocity
	newVelocity = velLast[gid] + deltaTime * sum;
	
	//------------------------------
	// velocity -> position
	f = velocity;
	sum = M16C1 * f;
	
	index = ((step) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C2,f,sum); //sum += M16C2 * f;
	
	index = ((step-1) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C3,f,sum); //sum += M16C3 * f;
	
	index = ((step-2) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C4,f,sum); //sum += M16C4 * f;
	
	index = ((step-3) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C5,f,sum); //sum += M16C5 * f;
	
	index = ((step-4) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C6,f,sum); //sum += M16C6 * f;
	
	index = ((step-5) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C7,f,sum); //sum += M16C7 * f;
	
	index = ((step-6) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C8,f,sum); //sum += M16C8 * f;
	
	index = ((step-7) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C9,f,sum); //sum += M16C9 * f;
	
	index = ((step-8) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C10,f,sum); //sum += M16C10 * f;
	
	index = ((step-9) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C11,f,sum); //sum += M16C11 * f;
	
	index = ((step-10) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C12,f,sum); //sum += M16C12 * f;

	index = ((step-11) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C13,f,sum); //sum += M16C13 * f;
	
	index = ((step-12) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C14,f,sum); //sum += M16C14 * f;
	
	index = ((step-13) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C15,f,sum); //sum += M16C15 * f;
	
	index = ((step-14) & 0xF) * numParticles + gid;
	f = velHistory[index];
	sum = fma(M16C16,f,sum); //sum += M16C16 * f;

	// Store corrected position
	newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);
	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}
