/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2000 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include <stdlib.h>
#include <math.h>
#include "Include/alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "Include/alError.h"
#include "Include/alSource.h"

ALAPI ALvoid ALAPIENTRY alGenSources(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALsource *Source;
	ALCdevice *Device;
	ALsizei i=0;

	Context = alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (n > 0)
		{
			Device = alcGetContextsDevice(Context);
	
			if (Device)
			{
				// Check that enough memory has been allocted in the 'sources' array for n Sources
				if (!IsBadWritePtr((void*)sources, n * sizeof(ALuint)))
				{
					// Check that the requested number of sources can be generated
					if ((Context->SourceCount + n) <= Device->MaxNoOfSources)
					{
						// Have we create any Sources yet ?
						if (!Context->Source)
						{
							// First Source to be created !
							Context->Source=malloc(sizeof(ALsource));
							if (Context->Source)
							{
								memset(Context->Source,0,sizeof(ALsource));

								sources[i]=(ALuint)Context->Source;

								Context->Source->valid=AL_TRUE;
								Context->Source->update1 |= SGENERATESOURCE;
						
								Context->Source->param[AL_CONE_INNER_ANGLE-AL_CONE_INNER_ANGLE].data.f=360.0;

								Context->Source->param[AL_CONE_OUTER_ANGLE-AL_CONE_INNER_ANGLE].data.f=360.0;

								Context->Source->param[AL_PITCH-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Context->Source->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[0]=0.0;
								Context->Source->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Context->Source->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Context->Source->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[0]=1.0;
								Context->Source->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Context->Source->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Context->Source->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[0]=0.0;
								Context->Source->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Context->Source->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Context->Source->param[AL_REFERENCE_DISTANCE-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Context->Source->param[AL_MAX_DISTANCE-AL_CONE_INNER_ANGLE].data.f= 1000000.0;
						
								Context->Source->param[AL_ROLLOFF_FACTOR-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Context->Source->param[AL_LOOPING-AL_CONE_INNER_ANGLE].data.i= AL_FALSE;

								Context->Source->param[AL_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Context->Source->param[AL_MIN_GAIN-AL_CONE_INNER_ANGLE].data.f= 0.0f;

								Context->Source->param[AL_MAX_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Context->Source->param[AL_CONE_OUTER_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Context->Source->state = AL_INITIAL;

								Context->Source->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i= 0;

								Context->Source->EAX20BP.lDirect = EAXBUFFER_DEFAULTDIRECT;
								Context->Source->EAX20BP.lDirectHF = EAXBUFFER_DEFAULTDIRECTHF;
								Context->Source->EAX20BP.lRoom = EAXBUFFER_DEFAULTROOM;
								Context->Source->EAX20BP.lRoomHF = EAXBUFFER_DEFAULTROOMHF;
								Context->Source->EAX20BP.lObstruction = EAXBUFFER_DEFAULTOBSTRUCTION;
								Context->Source->EAX20BP.flObstructionLFRatio = EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO;
								Context->Source->EAX20BP.lOcclusion = EAXBUFFER_DEFAULTOCCLUSION;
								Context->Source->EAX20BP.flOcclusionLFRatio = EAXBUFFER_DEFAULTOCCLUSIONLFRATIO;
								Context->Source->EAX20BP.flOcclusionRoomRatio = EAXBUFFER_DEFAULTOCCLUSIONROOMRATIO;
								Context->Source->EAX20BP.lOutsideVolumeHF = EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF;
								Context->Source->EAX20BP.flRoomRolloffFactor = EAXBUFFER_DEFAULTROOMROLLOFFFACTOR;
								Context->Source->EAX20BP.flAirAbsorptionFactor = EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR;
								Context->Source->EAX20BP.dwFlags = EAXBUFFER_DEFAULTFLAGS;  

								Context->Source->update1 |= CONEANGLES | FREQUENCY | POSITION | VELOCITY | ORIENTATION |
									MINDIST | MAXDIST | LOOPED | VOLUME | CONEOUTSIDEVOLUME | STATE | ROLLOFFFACTOR;

								Context->SourceCount++;
								i++;
							
								alcUpdateContext(Context, ALSOURCE, (ALuint)Context->Source);
							}

							Source=Context->Source;
						}
						else
						{
							// Some number of sources have already been created - move to the end of the list
							Source=Context->Source;
							while (Source->next)
								Source=Source->next;
						}

						// Add additional sources to the list (Source->next points to the location for the next Source structure)
						while ((Source)&&(i<n))
						{
							Source->next=malloc(sizeof(ALsource));
							if (Source->next)
							{
								memset(Source->next,0,sizeof(ALsource));
								
								sources[i]=(ALuint)Source->next;
								
								Source->next->previous=Source;
								Source->next->valid=AL_TRUE;
								Source->next->update1 |= SGENERATESOURCE;

								Source->next->param[AL_CONE_INNER_ANGLE-AL_CONE_INNER_ANGLE].data.f=360.0;

								Source->next->param[AL_CONE_OUTER_ANGLE-AL_CONE_INNER_ANGLE].data.f=360.0;

								Source->next->param[AL_PITCH-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Source->next->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[0]=0.0;
								Source->next->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Source->next->param[AL_POSITION-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Source->next->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[0]=1.0;
								Source->next->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Source->next->param[AL_DIRECTION-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Source->next->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[0]=0.0;
								Source->next->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[1]=0.0;
								Source->next->param[AL_VELOCITY-AL_CONE_INNER_ANGLE].data.fv3[2]=0.0;

								Source->next->param[AL_REFERENCE_DISTANCE-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Source->next->param[AL_MAX_DISTANCE-AL_CONE_INNER_ANGLE].data.f= 1000000.0;

								Source->next->param[AL_ROLLOFF_FACTOR-AL_CONE_INNER_ANGLE].data.f= 1.0;

								Source->next->param[AL_LOOPING-AL_CONE_INNER_ANGLE].data.i= AL_FALSE;

								Source->next->param[AL_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Source->next->param[AL_MIN_GAIN-AL_CONE_INNER_ANGLE].data.f= 0.0f;

								Source->next->param[AL_MAX_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Source->next->param[AL_CONE_OUTER_GAIN-AL_CONE_INNER_ANGLE].data.f= 1.0f;

								Source->next->state = AL_INITIAL;

								Source->next->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i= 0;

								Source->next->EAX20BP.lDirect = EAXBUFFER_DEFAULTDIRECT;
								Source->next->EAX20BP.lDirectHF = EAXBUFFER_DEFAULTDIRECTHF;
								Source->next->EAX20BP.lRoom = EAXBUFFER_DEFAULTROOM;
								Source->next->EAX20BP.lRoomHF = EAXBUFFER_DEFAULTROOMHF;
								Source->next->EAX20BP.lObstruction = EAXBUFFER_DEFAULTOBSTRUCTION;
								Source->next->EAX20BP.flObstructionLFRatio = EAXBUFFER_DEFAULTOBSTRUCTIONLFRATIO;
								Source->next->EAX20BP.lOcclusion = EAXBUFFER_DEFAULTOCCLUSION;
								Source->next->EAX20BP.flOcclusionLFRatio = EAXBUFFER_DEFAULTOCCLUSIONLFRATIO;
								Source->next->EAX20BP.flOcclusionRoomRatio = EAXBUFFER_DEFAULTOCCLUSIONROOMRATIO;
								Source->next->EAX20BP.lOutsideVolumeHF = EAXBUFFER_DEFAULTOUTSIDEVOLUMEHF;
								Source->next->EAX20BP.flRoomRolloffFactor = EAXBUFFER_DEFAULTROOMROLLOFFFACTOR;
								Source->next->EAX20BP.flAirAbsorptionFactor = EAXBUFFER_DEFAULTAIRABSORPTIONFACTOR;
								Source->next->EAX20BP.dwFlags = EAXBUFFER_DEFAULTFLAGS;

								Source->next->update1 |= CONEANGLES | FREQUENCY | POSITION | VELOCITY | ORIENTATION |
									MINDIST | MAXDIST | LOOPED | VOLUME | CONEOUTSIDEVOLUME | STATE | ROLLOFFFACTOR;

								Context->SourceCount++;
								i++;

								alcUpdateContext(Context, ALSOURCE, (ALuint)Source->next);

								Source=Source->next;
							}
							else
							{
								// Out of memory
								break;
							}
						}

						// If we didn't create all the Sources, we must have run out or memory
						if (i!=n)
						{
							alSetError(AL_OUT_OF_MEMORY);
						}
					}
					else
					{
						// Not enough resources to create the Sources
						alSetError(AL_INVALID_VALUE);
					}
				}
				else
				{
					// Bad pointer
					alSetError(AL_INVALID_VALUE);
				}
			}
			else
			{
				// No Device created, or attached to Context
				alSetError(AL_INVALID_OPERATION);
			}
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}



ALAPI ALvoid ALAPIENTRY alDeleteSources(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALCdevice  *Device;
	ALsource *ALSource;
	ALsizei i;
	ALbufferlistitem *ALBufferList;
	ALboolean bSourcesValid = AL_TRUE;

	Context = alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
	
		if (n > 0)
		{
			Device = alcGetContextsDevice(Context);
			
			if (Device)
			{
				if (n <= Context->SourceCount)
				{
					// Check that all Sources are valid (and can therefore be deleted)
					for (i = 0; i < n; i++)
					{
						if (!alIsSource(sources[i]))
						{
							alSetError(AL_INVALID_NAME);
							bSourcesValid = AL_FALSE;
							break;
						}
					}

					if (bSourcesValid)
					{
						// All Sources are valid, and can be deleted
						for (i = 0; i < n; i++)
						{
							// Recheck that the Source is valid, because there could be duplicated Source names
							if (alIsSource(sources[i]))
							{
								ALSource=((ALsource *)sources[i]);
								alSourceStop((ALuint)ALSource);

								// For each buffer in the source's queue, decrement its reference counter and remove it
								while (ALSource->queue != NULL)
								{
									ALBufferList = ALSource->queue;
									// Decrement buffer's reference counter
									if (ALBufferList->buffer != 0)
										((ALbuffer*)(ALBufferList->buffer))->refcount--;
									// Update queue to point to next element in list
									ALSource->queue = ALBufferList->next;
									// Release memory allocated for buffer list item
									free(ALBufferList);
								}

								// Call alcUpdateContext with SDELETE flag to perform context specific deletion of source
								ALSource->update1 = SDELETE;
								alcUpdateContext(Context, ALSOURCE, (ALuint)ALSource);

								// Decrement Source count
								Context->SourceCount--;

								// Remove Source from list of Sources
								if (ALSource->previous)
									ALSource->previous->next=ALSource->next;
								else
									Context->Source=ALSource->next;
								if (ALSource->next)
									ALSource->next->previous=ALSource->previous;

								memset(ALSource,0,sizeof(ALsource));
								free(ALSource);
							}
						}
					}
				}
				else
				{
					// Trying to delete more Sources than have been generated
					alSetError(AL_INVALID_NAME);
				}
			}
			else
			{
				// No Device created, or attached to Context
				alSetError(AL_INVALID_OPERATION);
			}


		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALboolean ALAPIENTRY alIsSource(ALuint source)
{
	ALboolean result=AL_FALSE;
	ALCcontext *Context;
	ALsource *Source;
	unsigned int i;
	
	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		// To determine if this is a valid Source name, look through the list of generated Sources
		Source = Context->Source;
		for (i = 0; i < Context->SourceCount; i++)
		{
			if ((ALuint)Source == source)
			{
				result = AL_TRUE;
				break;
			}

			Source = Source->next;
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return result;
}


ALAPI ALvoid ALAPIENTRY alSourcef(ALuint source,ALenum pname,ALfloat value)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			switch(pname)
			{
				case AL_PITCH:
					if ((value>=0.0f)&&(value<=2.0f))
					{
						if (value != Source->param[pname-AL_CONE_INNER_ANGLE].data.f)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.f=value;
							Source->update1 |= FREQUENCY;
							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				case AL_CONE_INNER_ANGLE:
				case AL_CONE_OUTER_ANGLE:
					if ((value>=0)&&(value<=360))
					{
						if (value != Source->param[pname-AL_CONE_INNER_ANGLE].data.f)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.f=value;
							Source->update1 |= CONEANGLES;
							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				case AL_GAIN:
				case AL_MAX_DISTANCE:
				case AL_ROLLOFF_FACTOR:
				case AL_REFERENCE_DISTANCE:
					if (value>=0.0f)
					{
						if (value != Source->param[pname-AL_CONE_INNER_ANGLE].data.f)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.f = value;

							if (pname == AL_GAIN)
								Source->update1 |= VOLUME;
							else if (pname == AL_MAX_DISTANCE)
								Source->update1 |= MAXDIST;
							else if (pname == AL_REFERENCE_DISTANCE)
								Source->update1 |= MINDIST;
							else if (pname == AL_ROLLOFF_FACTOR)
								Source->update1 |= ROLLOFFFACTOR;

							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				case AL_MIN_GAIN:
				case AL_MAX_GAIN:
				case AL_CONE_OUTER_GAIN:
					if ((value>=0.0f)&&(value<=1.0f))
					{	
						if (value != Source->param[pname-AL_CONE_INNER_ANGLE].data.f)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.f=value;

							// MIN_GAIN and MAX_GAIN unsupported at this time
							
							if (pname == AL_CONE_OUTER_GAIN)
							{
								Source->update1 |= CONEOUTSIDEVOLUME;
								alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
							}
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				default:
					alSetError(AL_INVALID_ENUM);
					break;
			}
		} 
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALvoid ALAPIENTRY alSourcefv(ALuint source,ALenum pname,ALfloat *values)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (values)
		{
			if (alIsSource(source))
			{
				Source=((ALsource *)source);
				switch(pname) 
				{
					case AL_POSITION:
					case AL_VELOCITY:
					case AL_DIRECTION:
						if ((values[0] != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0])||
							(values[1] != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1])||
							(values[2] != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2]))
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0] = values[0];
							Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1] = values[1];
							Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2] = values[2];

							if (pname == AL_POSITION)
								Source->update1 |= POSITION;
							else if (pname == AL_VELOCITY)
								Source->update1 |= VELOCITY;
							else if (pname == AL_DIRECTION)
								Source->update1 |= ORIENTATION;

							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
						break;

					default:
						alSetError(AL_INVALID_ENUM);
						break;
				}
			}
			else
			{
				// Invalid Source name
				alSetError(AL_INVALID_NAME);
			}
		}
		else
		{
			// values is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALvoid ALAPIENTRY alSource3f(ALuint source,ALenum pname,ALfloat v1,ALfloat v2,ALfloat v3)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
	
		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			switch(pname) 
			{
				case AL_POSITION:
				case AL_VELOCITY:
				case AL_DIRECTION:
					if ((v1 != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0])||
						(v2 != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1])||
						(v3 != Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2]))
					{
						Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0] = v1;
						Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1] = v2;
						Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2] = v3;

						if (pname == AL_POSITION)
							Source->update1 |= POSITION;
						else if (pname == AL_VELOCITY)
							Source->update1 |= VELOCITY;
						else if (pname == AL_DIRECTION)
							Source->update1 |= ORIENTATION;

						alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
					}
					break;
				default:
					alSetError(AL_INVALID_ENUM);
					break;
			}
		}
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALvoid ALAPIENTRY alSourcei(ALuint source,ALenum pname,ALint value)
{
	ALCcontext *Context;
	ALsource *Source;
	ALbufferlistitem *ALBufferListItem;
	ALint	Counter = 0;
	ALint	DataSize = 0;
	ALint	BufferSize;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
	
		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			switch(pname) 
			{
				case AL_SOURCE_RELATIVE:
					if ((value==AL_FALSE)||(value==AL_TRUE))
					{
						if (value != Source->relative)
						{
							Source->relative=value;
							Source->update1 |= MODE;
							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				case AL_CONE_INNER_ANGLE:
				case AL_CONE_OUTER_ANGLE:
					if ((value>=0)&&(value<=360))
					{
						if (((float)value) != Source->param[pname-AL_CONE_INNER_ANGLE].data.f)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.f = (float)value;
							Source->update1 |= CONEANGLES;
							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;
				
				case AL_LOOPING:
					if ((value==AL_FALSE)||(value==AL_TRUE))
					{
						if (value != Source->param[pname-AL_CONE_INNER_ANGLE].data.i)
						{
							Source->param[pname-AL_CONE_INNER_ANGLE].data.i = value;
							Source->update1 |= LOOPED;
							alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
						}
					}
					else
					{
						alSetError(AL_INVALID_VALUE);
					}
					break;

				case AL_BUFFER:
					if ((Source->state == AL_STOPPED) || (Source->state == AL_INITIAL))
					{
						if (alIsBuffer(value) || (value == 0))
						{
							// Remove all elements in the queue
							while (Source->queue != NULL)
							{
								ALBufferListItem = Source->queue;
								Source->queue = ALBufferListItem->next;
								// Decrement reference counter for buffer
								if (ALBufferListItem->buffer)
									((ALbuffer*)(ALBufferListItem->buffer))->refcount--;
								// Record size of buffer
								BufferSize = ((ALbuffer*)ALBufferListItem->buffer)->size;
								DataSize += BufferSize;
								// Increment the number of buffers removed from queue
								Counter++;
								// Release memory for buffer list item
								free(ALBufferListItem);
								// Decrement the number of buffers in the queue
								Source->BuffersInQueue--;
							}

							// Update variables required by the SUNQUEUE routine in UpdateContext
							Source->NumBuffersRemovedFromQueue = Counter;
							Source->SizeOfBufferDataRemovedFromQueue = DataSize;
							Source->update1 |= SUNQUEUE;
							alcUpdateContext(Context, ALSOURCE, source);

							// Add the buffer to the queue (as long as it is NOT the NULL buffer, AND it is
							// more than 0 bytes long)
							if (value != 0)
							{
								// Add the selected buffer to the queue
								ALBufferListItem = malloc(sizeof(ALbufferlistitem));
								ALBufferListItem->buffer = value;
								ALBufferListItem->bufferstate = PENDING;
								ALBufferListItem->flag = 0;
								ALBufferListItem->next = NULL;

								Source->queue = ALBufferListItem;
								Source->BuffersInQueue = 1;
								
								DataSize = ((ALbuffer*)value)->size;

								// Increment reference counter for buffer
								((ALbuffer*)(value))->refcount++;

								Source->SizeOfBufferDataAddedToQueue = DataSize;
								Source->NumBuffersAddedToQueue = 1;
								Source->update1 |= SQUEUE;
								alcUpdateContext(Context, ALSOURCE, source);
							}

							// Set Buffers Processed
							Source->BuffersProcessed = 0;

							// Update AL_BUFFER parameter
							Source->param[pname-AL_CONE_INNER_ANGLE].data.i=value;
						}
						else
						{
							alSetError(AL_INVALID_NAME);
						}
					}
					else
					{
						alSetError(AL_INVALID_OPERATION);
					}
					break;

				case AL_SOURCE_STATE:
					Source->state=value;
					Source->update1 |= STATE;
					alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
					break;

				default:
					alSetError(AL_INVALID_ENUM);
					break;
			}
		}
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alGetSourcef(ALuint source,ALenum pname,ALfloat *value)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (value)
		{
			if (alIsSource(source))
			{
				Source=((ALsource *)source);
				switch(pname) 
				{
					case AL_PITCH:
					case AL_GAIN:
					case AL_MIN_GAIN:
					case AL_MAX_GAIN:
					case AL_MAX_DISTANCE:
					case AL_ROLLOFF_FACTOR:
					case AL_CONE_OUTER_GAIN:
					case AL_CONE_INNER_ANGLE:
					case AL_CONE_OUTER_ANGLE:
					case AL_REFERENCE_DISTANCE:
						*value=Source->param[pname-AL_CONE_INNER_ANGLE].data.f;
						break;
					default:
						alSetError(AL_INVALID_ENUM);
						break;
				}
			}
			else
			{
				// Invalid Source name
				alSetError(AL_INVALID_NAME);
			}
		}
		else
		{
			// value is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALvoid ALAPIENTRY alGetSource3f(ALuint source, ALenum pname, ALfloat* v1, ALfloat* v2, ALfloat* v3)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
		
		if ((v1) && (v2) && (v3))
		{
			if (alIsSource(source))
			{
				Source=((ALsource *)source);
				switch(pname) 
				{
				case AL_POSITION:
				case AL_VELOCITY:
				case AL_DIRECTION:
					*v1 = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0];
					*v2 = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1];
					*v3 = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2];
					break;

				default:
					alSetError(AL_INVALID_ENUM);
					break;
				}
			} 
			else
			{
				// Invalid Source Name
				alSetError(AL_INVALID_NAME);
			}
		}
		else
		{
			// Invalid pointer(s)
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}
	
	return;
}


ALAPI ALvoid ALAPIENTRY alGetSourcefv(ALuint source,ALenum pname,ALfloat *values)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
		
		if (values)
		{
			if (alIsSource(source))
			{
				Source=((ALsource *)source);
				switch(pname) 
				{
				case AL_POSITION:
				case AL_VELOCITY:
				case AL_DIRECTION:
					values[0] = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[0];
					values[1] = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[1];
					values[2] = Source->param[pname-AL_CONE_INNER_ANGLE].data.fv3[2];
					break;

				default:
					alSetError(AL_INVALID_ENUM);
					break;
				}
			} 
			else
			{
				// Invalid Source Name
				alSetError(AL_INVALID_NAME);
			}
		}
		else
		{
			// values is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}
	
		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alGetSourcei(ALuint source,ALenum pname,ALint *value)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (value)
		{
			if (alIsSource(source))
			{
				Source=((ALsource *)source);
				switch(pname) 
				{
					case AL_SOURCE_RELATIVE:
						*value = Source->relative;
						break;

					case AL_CONE_INNER_ANGLE:
					case AL_CONE_OUTER_ANGLE:
						*value = (ALint)Source->param[pname-AL_CONE_INNER_ANGLE].data.f;
						break;

					case AL_LOOPING:
						*value = Source->param[pname-AL_CONE_INNER_ANGLE].data.i;
						break;

					case AL_BUFFER:
						*value = Source->param[pname-AL_CONE_INNER_ANGLE].data.i;
						break;

					case AL_SOURCE_STATE:
						*value=Source->state;
						break;

					case AL_BUFFERS_QUEUED:
						*value=Source->BuffersInQueue;
						break;

					case AL_BUFFERS_PROCESSED:
						*value=Source->BuffersProcessed;
						break;

					default:
						alSetError(AL_INVALID_ENUM);
						break;
				}
			} 
			else
			{
				// Invalid Source Name
				alSetError(AL_INVALID_NAME);
			}
		}
		else
		{
			// value is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}
		
		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourcePlay(ALuint source)
{
	ALCcontext *Context;
	ALsource *Source;
	ALbufferlistitem *ALBufferList;
	
	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			if (Source->state!=AL_PAUSED)
			{
				Source->state=AL_PLAYING;
				Source->inuse=AL_TRUE;
				Source->play=AL_TRUE;
				Source->position=0;
				Source->position_fraction=0;
				Source->BuffersProcessed = 0;
				Source->BuffersAddedToDSBuffer = 0;
				
				if (Source->queue)
					Source->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = Source->queue->buffer;
				
				// Make sure all the Buffers in the queue are marked as PENDING
				ALBufferList = Source->queue;
				while (ALBufferList)
				{
					ALBufferList->bufferstate = PENDING;
					ALBufferList = ALBufferList->next;
				}
			}
			else
			{
				Source->state=AL_PLAYING;
				Source->inuse=AL_TRUE;
				Source->play=AL_TRUE;
			}
			Source->update1 |= STATE;
			alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
		} 
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourcePlayv(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALsource *Source;
	ALbufferlistitem *ALBufferList;
	ALsizei i;
	ALboolean bSourcesValid = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (sources)
		{
			// Check that all the Sources are valid
			for (i=0;i<n;i++)
			{
				if (!alIsSource(sources[i]))
				{
					alSetError(AL_INVALID_NAME);
					bSourcesValid = AL_FALSE;
					break;
				}
			}

			if (bSourcesValid)
			{
				for (i=0;i<n;i++)
				{
					Source=((ALsource *)sources[i]);
					if (Source->state!=AL_PAUSED)
					{
						Source->state=AL_PLAYING;
						Source->inuse=AL_TRUE;
						Source->play=AL_TRUE;
						Source->position=0;
						Source->position_fraction=0;
						Source->BuffersProcessed = 0;
						Source->BuffersAddedToDSBuffer = 0;
						if (Source->queue)
							Source->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = Source->queue->buffer;

						// Make sure all the Buffers in the queue are marked as PENDING
						ALBufferList = Source->queue;
						while (ALBufferList)
						{
							ALBufferList->bufferstate = PENDING;
							ALBufferList = ALBufferList->next;
						}
					}
					else
					{
						Source->state=AL_PLAYING;
						Source->inuse=AL_TRUE;
						Source->play=AL_TRUE;
					}
					Source->update1 |= STATE;
					alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
				}
			}
		}
		else
		{
			// sources is a NULL pointer
			alSetError(AL_INVALID_VALUE);

		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourcePause(ALuint source)
{
	ALCcontext *Context;
	ALsource *Source;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
	
		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			if (Source->state==AL_PLAYING)
			{
				Source->state=AL_PAUSED;
				Source->inuse=AL_FALSE;
			}
			Source->update1 = STATE;
			alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
		} 
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourcePausev(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALsource *Source;
	ALsizei i;
	ALboolean bSourcesValid = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (sources)
		{
			// Check all the Sources are valid
			for (i=0;i<n;i++)
			{
				if (!alIsSource(sources[i]))
				{
					alSetError(AL_INVALID_NAME);
					bSourcesValid = AL_FALSE;
					break;
				}
			}

			if (bSourcesValid)
			{
				for (i=0;i<n;i++)
				{
					Source=((ALsource *)sources[i]);
					if (Source->state==AL_PLAYING)
					{
						Source->state=AL_PAUSED;
						Source->inuse=AL_FALSE;
					}
					Source->update1 |= STATE;
					alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
				}
			}
		}
		else
		{
			// sources is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourceStop(ALuint source)
{
	ALCcontext *Context;
	ALsource *Source;
	ALbufferlistitem *ALBufferListItem;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);
	
		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			if (Source->state!=AL_INITIAL)
			{
				Source->state=AL_STOPPED;
				Source->inuse=AL_FALSE;
				Source->BuffersAddedToDSBuffer = Source->BuffersProcessed = Source->BuffersInQueue;
				ALBufferListItem= Source->queue;
				while (ALBufferListItem != NULL)
				{
					ALBufferListItem->bufferstate = PROCESSED;
					ALBufferListItem = ALBufferListItem->next;
				}
			}
			Source->update1 |= STATE;
			alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
		} 
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}
	
		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourceStopv(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALsource *Source;
	ALsizei i;
	ALbufferlistitem *ALBufferListItem;
	ALboolean bSourcesValid = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (sources)
		{
			// Check all the Sources are valid
			for (i=0;i<n;i++)
			{
				if (!alIsSource(sources[i]))
				{
					alSetError(AL_INVALID_NAME);
					bSourcesValid = AL_FALSE;
					break;
				}
			}

			if (bSourcesValid)
			{
				for (i=0;i<n;i++)
				{
					Source=((ALsource *)sources[i]);
					if (Source->state!=AL_INITIAL)
					{
						Source->state=AL_STOPPED;
						Source->inuse=AL_FALSE;
						Source->BuffersAddedToDSBuffer = Source->BuffersProcessed = Source->BuffersInQueue;
						ALBufferListItem= Source->queue;
						while (ALBufferListItem != NULL)
						{
							ALBufferListItem->bufferstate = PROCESSED;
							ALBufferListItem = ALBufferListItem->next;
						}
					}
					Source->update1 |= STATE;
					alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
				}
			}
		}
		else
		{
			// sources is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourceRewind(ALuint source)
{
	ALCcontext *Context;
	ALsource *Source;
	ALbufferlistitem *ALBufferListItem;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (alIsSource(source))
		{
			Source=((ALsource *)source);
			if (Source->state!=AL_INITIAL)
			{
				Source->state=AL_INITIAL;
				Source->inuse=AL_FALSE;
				Source->position=0;
				Source->position_fraction=0;
				Source->BuffersProcessed = 0;
				ALBufferListItem= Source->queue;
				while (ALBufferListItem != NULL)
				{
					ALBufferListItem->bufferstate = PROCESSED;
					ALBufferListItem = ALBufferListItem->next;
				}
				if (Source->queue)
					Source->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = Source->queue->buffer;
			}
			Source->update1 |= STATE;
			alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
		} 
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}

ALAPI ALvoid ALAPIENTRY alSourceRewindv(ALsizei n,ALuint *sources)
{
	ALCcontext *Context;
	ALsource *Source;
	ALsizei i;
	ALbufferlistitem *ALBufferListItem;
	ALboolean bSourcesValid = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (sources)
		{	
			// Check all the Sources are valid
			for (i=0;i<n;i++)
			{
				if (!alIsSource(sources[i]))
				{
					alSetError(AL_INVALID_NAME);
					bSourcesValid = AL_FALSE;
					break;
				}
			}

			if (bSourcesValid)
			{
				for (i=0;i<n;i++)
				{
					Source=((ALsource *)sources[i]);
					if (Source->state!=AL_INITIAL)
					{
						Source->state=AL_INITIAL;
						Source->inuse=AL_FALSE;
						Source->position=0;
						Source->position_fraction=0;
						Source->BuffersProcessed = 0;
						ALBufferListItem= Source->queue;
						while (ALBufferListItem != NULL)
						{
							ALBufferListItem->bufferstate = PROCESSED;
							ALBufferListItem = ALBufferListItem->next;
						}
						if (Source->queue)
							Source->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = Source->queue->buffer;
					}
					Source->update1 |= STATE;
					alcUpdateContext(Context, ALSOURCE, (ALuint)Source);
				}
			}
		}
		else
		{
			// sources is a NULL pointer
			alSetError(AL_INVALID_VALUE);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


ALAPI ALvoid ALAPIENTRY alSourceQueueBuffers( ALuint source, ALsizei n, ALuint* buffers )
{
	ALCcontext *Context;
	ALsource *ALSource;
	ALsizei i;
	ALbufferlistitem *ALBufferList;
	ALbufferlistitem *ALBufferListStart;
	ALuint DataSize;
	ALuint BufferSize;
	ALboolean bBuffersValid = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		ALSource = (ALsource*)source;

		DataSize = 0;
		BufferSize = 0;

		// Check that all buffers are valid or zero and that the source is valid
		
		// Check that this is a valid source
		if (alIsSource(source))
		{
			for (i = 0; i < n; i++)
			{
				if ((!alIsBuffer(buffers[i])) && (buffers[i] != 0))
				{
					alSetError(AL_INVALID_NAME);
					bBuffersValid = AL_FALSE;
					break;
				}
			}

			if (bBuffersValid)
			{
				// All buffers are valid - so add them to the list
				ALBufferListStart = malloc(sizeof(ALbufferlistitem));
				ALBufferListStart->buffer = buffers[0];
				ALBufferListStart->bufferstate = PENDING;
				ALBufferListStart->flag = 0;
				ALBufferListStart->next = NULL;

				if (buffers[0])
					BufferSize = ((ALbuffer*)buffers[0])->size;
				else
					BufferSize = 0;

				DataSize += BufferSize;

				// Increment reference counter for buffer
				if (buffers[0])
					((ALbuffer*)(buffers[0]))->refcount++;

				ALBufferList = ALBufferListStart;

				for (i = 1; i < n; i++)
				{
					ALBufferList->next = malloc(sizeof(ALbufferlistitem));
					ALBufferList->next->buffer = buffers[i];
					ALBufferList->next->bufferstate = PENDING;
					ALBufferList->next->flag = 0;
					ALBufferList->next->next = NULL;

					if (buffers[i])
						BufferSize = ((ALbuffer*)buffers[i])->size;
					else
						BufferSize = 0;

					DataSize += BufferSize;

					// Increment reference counter for buffer
					if (buffers[i])
						((ALbuffer*)(buffers[i]))->refcount++;
				
					ALBufferList = ALBufferList->next;
				}

				if (ALSource->queue == NULL)
				{
					ALSource->queue = ALBufferListStart;		
					// Update Current Buffer
					ALSource->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = ALBufferListStart->buffer;
				}
				else
				{
					// Find end of queue
					ALBufferList = ALSource->queue;
					while (ALBufferList->next != NULL)
					{
						ALBufferList = ALBufferList->next;
					}

					ALBufferList->next = ALBufferListStart;
				}

				// Update number of buffers in queue
				ALSource->BuffersInQueue += n;

				// Record the amount of data added to the queue
				ALSource->SizeOfBufferDataAddedToQueue = DataSize;
				ALSource->NumBuffersAddedToQueue = n;
				ALSource->update1 |= SQUEUE;
				alcUpdateContext(Context, ALSOURCE, source);
			}
		}
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}


// Implementation assumes that n is the number of buffers to be removed from the queue and buffers is
// an array of buffer IDs that are to be filled with the names of the buffers removed
ALAPI ALvoid ALAPIENTRY alSourceUnqueueBuffers( ALuint source, ALsizei n, ALuint* buffers )
{
	ALCcontext *Context;
	ALsource *ALSource;
	ALsizei i;
	ALbufferlistitem *ALBufferList;
	ALuint DataSize;
	ALuint BufferSize;
	ALuint BufferID;
	ALboolean bBuffersProcessed;

	DataSize = 0;
	BufferSize = 0;
	bBuffersProcessed = AL_TRUE;

	Context=alcGetCurrentContext();
	if (Context)
	{
		SuspendContext(Context);

		if (alIsSource(source))
		{
			ALSource = (ALsource*)source;

			// Check that all 'n' buffers have been processed
			ALBufferList = ALSource->queue;
			for (i = 0; i < n; i++)
			{
				if ((ALBufferList != NULL) && (ALBufferList->bufferstate == PROCESSED))
				{
					ALBufferList = ALBufferList->next;
				}
				else
				{
					bBuffersProcessed = AL_FALSE;
					break;
				}
			}

			// If all 'n' buffers have been processed, remove them from the queue
			if (bBuffersProcessed)
			{
				for (i = 0; i < n; i++)
				{
					ALBufferList = ALSource->queue;

					ALSource->queue = ALBufferList->next;
					// Record name of buffer
					buffers[i] = ALBufferList->buffer;
					// Decrement buffer reference counter
					if (ALBufferList->buffer)
						((ALbuffer*)(ALBufferList->buffer))->refcount--;
					// Record size of buffer
					if (ALBufferList->buffer)
						BufferSize = ((ALbuffer*)ALBufferList->buffer)->size;
					else
						BufferSize = 0;

					DataSize += BufferSize;
					// Release memory for buffer list item
					free(ALBufferList);
					ALSource->BuffersInQueue--;
					ALSource->BuffersProcessed--;
				}

				if (ALSource->state != AL_PLAYING)
				{
					if (ALSource->queue)
						BufferID = ALSource->queue->buffer;
					else
						BufferID = 0;

					ALSource->param[AL_BUFFER-AL_CONE_INNER_ANGLE].data.i = BufferID;
				}

				ALSource->NumBuffersRemovedFromQueue = n;
				ALSource->SizeOfBufferDataRemovedFromQueue = DataSize;
				ALSource->BuffersAddedToDSBuffer -= ALSource->NumBuffersRemovedFromQueue;

				ALSource->update1 |= SUNQUEUE;
				alcUpdateContext(Context, ALSOURCE, source);
			}
			else
			{
				// Some buffers can't be unqueue because they have not been processed
				alSetError(AL_INVALID_OPERATION);
			}
		}
		else
		{
			// Invalid Source Name
			alSetError(AL_INVALID_NAME);
		}

		ProcessContext(Context);
	}
	else
	{
		// Invalid Context
		alSetError(AL_INVALID_OPERATION);
	}

	return;
}