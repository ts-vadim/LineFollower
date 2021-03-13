#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Arduino.h>
#include <EEPROM.h>

class ResourceManager
{
public:
	template <typename T>
	void Get(void* address, T* container);

	template <typename T>
	void Put(void* address, T& object);
};

#endif
