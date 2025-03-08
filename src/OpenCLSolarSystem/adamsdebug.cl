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
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

__kernel
void computeAcc( 
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
	
	bool doPrintf = (gid == -1);
	
	// Do the Sun
	r = gravPos[0] - myPos;
	r.w =0.0;
	distSqr = r.x * r.x + r.y * r.y + r.z * r.z;
	invDist = rsqrt(distSqr + epsSqr); 
	invDistCube = invDist * invDist * invDist; 
	s = gravPos[0].w * invDistCube; 
	double4 accSun= s * r; 
	if(doPrintf)
	{
		printf( "distSqr %.16e grav %.16e\n",distSqr,gravPos[0].w);
	}
	
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
void move( 
__global double4* pos, 
__global double4* vel,
__global double4* acc, 
double deltaTime, 
__global double4* newPosition, 
__global double4* newVelocity,
int stage,
int step,
int numParticles,
__global double4* posLast,
__global double4* velLast,
__global double4* velHistory,
__global double4* accHistory)
{
	unsigned int gid = get_global_id(0);
	double4 position;
	double4 velocity;
	double4 acceleration;
	double4 newPos;
	double4 newVel;
	if( stage == 1)
	{
		position = pos[gid]; 
		velocity = vel[gid];
		acceleration = acc[gid];
		long index = ((step-1) & 0xF) * numParticles + gid;
		velHistory[index]=velocity;
		accHistory[index]=acceleration;
		
		newPos = position + velocity * deltaTime * 0.5 + acceleration *(deltaTime * 0.5)*(deltaTime * 0.5);
		newPos.w = position.w;
		newVel = velocity + acceleration * deltaTime * 0.5;
		posLast[gid] = position;
		velLast[gid] = velocity;
	}
	else
	{
		newPos = posLast[gid] + vel[gid] * deltaTime;
		newPos.w = position.w;
		newVel = velLast[gid] + acc[gid] * deltaTime;
	}

	newPosition[gid] = newPos; 
	newVelocity[gid] = newVel; 
}

#define KMTOGM 1.0/1000000

#define B2C1 1.500000000000000000
#define B2C2 -0.50000000000000000
#define M2C1 0.500000000000000000
#define M2C2 0.500000000000000000

#define B4C1 2.25
#define B4C2 -2.45833333333333
#define B4C3 1.54166666666667
#define B4C4 -0.375

#define M4C1 0.375
#define M4C2 0.791666666666667
#define M4C3 -0.208333333333333
#define M4C4 0.0416666666666667

#define B8C1 3.58995535714286
#define B8C2 -9.52520667989418
#define B8C3 18.0545386904762
#define B8C4 -22.0277529761905
#define B8C5 17.3796544312169
#define B8C6 -8.61212797619048
#define B8C7 2.44516369047619
#define B8C8 -0.304224537037037

#define M8C1 0.304224537037037
#define M8C2 1.15615906084656
#define M8C3 -1.00691964285714
#define M8C4 1.01796461640212
#define M8C5 -0.732035383597884
#define M8C6 0.343080357142857
#define M8C7 -0.0938409391534391
#define M8C8 0.0113673941798942


#define B12C1 4.726253940487881460
#define B12C2 -20.285746606065616482
#define B12C3 64.335095315965541659
#define B12C4 -141.522864179368085618
#define B12C5 223.526764175735529902
#define B12C6 -258.602100049352653519
#define B12C7 220.357899950647346480
#define B12C8 -137.124664395693041526
#define B12C9 60.739992963489057239
#define B12C10 -18.173476112605886911
#define B12C11 3.297110536791526374
#define B12C12 -0.274265540031599059

#define M12C1 0.274265540031599059
#define M12C2 1.435067460108692747
#define M12C3 -2.184220963980078563
#define M12C4 3.996676509013748597
#define M12C5 -5.761421863726551226
#define M12C6 6.308456470709074875
#define M12C7 -5.180741060155122655
#define M12C8 3.139592245620891454
#define M12C9 -1.363222080051507134
#define M12C10 0.401574156537264176
#define M12C11 -0.071950470520348992
#define M12C12 0.005924056412337662

#define B16C1 5.776080028330126933
#define B16C2 -34.437212290517538206
#define B16C3 153.295313563544223903
#define B16C4 -487.624165234214119606
#define B16C5 1155.966252758585158844
#define B16C6 -2102.383615302252961827
#define B16C7 2986.586096092599207006
#define B16C8 -3345.962146803640596785
#define B16C9 2964.670665491647888979
#define B16C10 -2069.993131887207256044
#define B16C11 1126.751450793321070254
#define B16C12 -468.674127888398662742
#define B16C13 143.998829364498292156
#define B16C14 -30.818758034463882040
#define B16C15 4.104778844743438324
#define B16C16 -0.256309496574389152

#define M16C1 0.256309496574389152
#define M16C2 1.675128083139900493
#define M16C3 -3.680072701590839904
#define M16C4 9.761995481886298495
#define M16C5 -21.140881468825862030
#define M16C6 36.406371721653340663
#define M16C7 -49.857166734544628493
#define M16C8 54.405455281587302244
#define M16C9 -47.258925891252203927
#define M16C10 32.490024680635984217
#define M16C11 -17.466683319498922711
#define M16C12 7.191569756389252073
#define M16C13 -2.190844123010405166
#define M16C14 0.465511282840366748
#define M16C15 -0.061618445537183739
#define M16C16 0.003826899553211884

__kernel
void adamsBashforthMoulton( 
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
	
	int order = step > 1? (step > 16? 8 : 2) : 1;
	
	bool doPrintf = (gid == -1);
	
	if(doPrintf)
	{
		printf("step %d stage: %d gid %u deltaTime: %.16e\n",step, stage,gid, deltaTime);
		printf(" pos: %.16v4e\n", position);
		printf(" vel: %.16v4e\n", velocity);
		printf(" acc: %.16v4e\n", acceleration);
	}
	
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
			sum += B2C2 * f;
			if(doPrintf)
			{
				printf("  AB (step-1)& 0xF: %d index: %ld\n",(step-1) & 0xF,index);
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
		}
		else
		{
			f = acceleration;
			sum = B12C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C2 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C3 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-3) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C4 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			
			index = ((step-4) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C5 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-5) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C6 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-6) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C7 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-7) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C8 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-8) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C9 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-9) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C10 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-10) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C11 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-11) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B12C12 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			/*
			index = ((step-12) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B16C13 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-13) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B16C14 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-14) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B16C15 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-15) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += B16C16 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			*/
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
			sum += B2C2 * f;
			if(doPrintf)
			{
				printf("  AB (step-1)& 0xF: %d index: %ld\n",(step-1) & 0xF,index);
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
		}
		else
		{
			f = velocity;
			sum = B12C1 * f;
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C2 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C3 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-3) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C4 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-4) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C5 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-5) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C6 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-6) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C7 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-7) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C8 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-8) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C9 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-9) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C10 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-10) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C11 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-11) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B12C12 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			/*
			index = ((step-12) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B16C13 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-13) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B16C14 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-14) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B16C15 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-15) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += B16C16 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			*/
		}
		
		newPosition = position + deltaTime * sum * (KMTOGM);// + 0.5*deltaTime*deltaTime*acceleration;
		posLast[gid] = position;
		
		index = ((step) & 0xF) * numParticles + gid;
		velocity.w = (double) step;
		velHistory[index] = velocity;
		acceleration.w = (double)step;
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
			sum += M2C2 * f;
			if(doPrintf)
			{
				printf("  AM (step-1)& 0xF: %d index: %ld\n",(step-1) & 0xF,index);
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
		}
		else
		{
			f = acceleration;
			sum = M12C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C2 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C3 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C4 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-3) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C5 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-4) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C6 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-5) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C7 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-6) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C8 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-7) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C9 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-8) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C10 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-9) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C11 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-10) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M12C12 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			/*
			index = ((step-11) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M16C13 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-12) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M16C14 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-13) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M16C15 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-14) & 0xF) * numParticles + gid;
			f = accHistory[index];
			sum += M16C16 * f;
			
			if(doPrintf)
			{
				printf("   accHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			*/
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
			sum += M2C2 * f;
			if(doPrintf)
			{
				printf("  AM (step-1)& 0xF: %d index: %ld\n",(step-1) & 0xF,index);
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
		}
		else
		{		
			f = velocity;
			sum = M12C1 * f;
			
			index = ((step) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C2 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-1) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C3 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-2) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C4 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-3) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C5 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-4) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C6 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-5) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C7 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-6) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C8 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-7) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C9 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-8) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C10 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-9) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C11 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-10) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M12C12 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			/*
			index = ((step-11) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M16C13 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-12) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M16C14 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-13) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M16C15 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			
			index = ((step-14) & 0xF) * numParticles + gid;
			f = velHistory[index];
			sum += M16C16 * f;
			
			if(doPrintf)
			{
				printf("   velHistory: %.16v4e\n",f);
				printf("   sum: %.16v4e\n",sum);
			}
			*/
		}
		
		// Store corrected position
		newPosition = posLast[gid] + deltaTime * sum * (KMTOGM);// + 0.5*deltaTime*deltaTime*acceleration;
	}
	


	
	// Copy across mass and relativistic parameter
	newPosition.w = position.w;
	newVelocity.w = velocity.w;
	
	if(doPrintf)
	{
		printf(" newPos: %.16v4e\n", newPosition);
		printf(" newVel: %.16v4e\n", newVelocity);
	}
		
	newPos[gid] = newPosition;
	newVel[gid] = newVelocity;
}