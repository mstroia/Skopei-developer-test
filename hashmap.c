#include "hashmap.h"

int hash(HashMap *hm, char *key)
{
	int sum = 0;
	int len = strlen(key) + 1;

	for (int i = 0; i < len; i++)
	{
		sum = sum + key[i];
	}

	return sum % hm->size;
}

HashMap *create_hashmap(size_t key_size)
{
	// check if key is larger than 0, if not don't create the map
	assert(key_size > 0);

	// allocate the memory for the map
	HashMap *hm = (HashMap *)malloc(sizeof(HashMap));
	// check it got allocated
	assert(hm);

	// allocate memory for the buckets and set all the bytes to 0
	hm->buckets = calloc(key_size, sizeof(Bucket));
	// check it got allocated
	assert(hm->buckets);

	// store the map size in the hashmap data structure
	hm->size = key_size;

	return hm;
}

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision)
{
	if (hm==NULL) {
		return;
	}
	int hash_value = hash(hm, key);
	// if this hash was never used, initialize the bucket
	if (!hm->buckets[hash_value])
	{
		hm->buckets[hash_value] = (Bucket*)malloc(sizeof(Bucket)); // this way only the actually needed buckets get initialised, saving memory
		hm->buckets[hash_value]->data = data;
		hm->buckets[hash_value]->key = key;
	}
	else
	{

		// check all buckets to see if the key appeared before
		Bucket *current_bucket = hm->buckets[hash_value];
		Bucket *previous_bucket = hm->buckets[hash_value];
		while (current_bucket != NULL)
		{
			if (memcmp(current_bucket->key, key, strlen(key)) == 0)
			{
				//if the key is found, assign the data field with the return value of the resolve_collision function
				current_bucket->data = resolve_collision(current_bucket->data, data);
				return;
			}
			else
			{
				//check next bucket
				previous_bucket = current_bucket;
				current_bucket = current_bucket->next;
			}
		}
		//if the key is not there, create a new bucket for the key and data
		previous_bucket->next = (Bucket*)malloc(sizeof(Bucket));
		previous_bucket->next->data = data;
		previous_bucket->next->key = key;
	}
}

void *get_data(HashMap *hm, char *key)
{
	if (hm==NULL) {
		return NULL;
	}
	int hash_value = hash(hm, key);
	//if the bucket exists(if not, clearly nothing got added)
	if (hm->buckets[hash_value])
	{
		//traverse the list of buckets until the key is found
		Bucket *current_bucket = hm->buckets[hash_value];
		while (current_bucket != NULL)
		{
			//if the key is found return the data
			if (memcmp(current_bucket->key, key, strlen(key)) == 0)
			{
				
				return current_bucket->data;
			}
			//if the data is not found, go to the next bucket in list
			current_bucket = current_bucket->next;
		}
	}
	return NULL;
}

void iterate(HashMap *hm, void (*callback)(char *key, void *data))
{
	if (hm==NULL) {
		return;
	}
	//iterate through each list of buckets
	for (size_t i = 0; i < hm->size; i++)
	{
		//if a bucket has been created at an index, iterate through it
		if (hm->buckets[i] != NULL)
		{
			Bucket *bucket = hm->buckets[i];
			while (bucket != NULL)
			{
				callback(bucket->key, bucket->data);
				bucket = bucket->next;
			}
		}
	}
}

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data)
{
	if (hm==NULL) {
		return;
	}
	//find the bucket
	int hash_value = hash(hm, key);
	if (hm->buckets[hash_value])
	{
		//iterate the list to find the exact key, until reaching the end
		Bucket *current_bucket = hm->buckets[hash_value];
		Bucket *previous_bucket = NULL;
		while (current_bucket)
		{
			if (memcmp(current_bucket->key, key, strlen(key)) == 0)
			{
				//if the key is found, use the destroy data function
				if (destroy_data)
				{
					destroy_data(current_bucket->data);
				}
				//before deleting the bucket, make sure the list of buckets stays intact
				if (previous_bucket)
				{
					previous_bucket->next = current_bucket->next;
				}
				//if the bucket is the first in the list, make the hashmap itself point to the next bucket
				else{
					hm->buckets[hash_value] = current_bucket->next;
				}
				//set everything in the bucket to null, and free the bucket
				current_bucket->data = NULL;
				current_bucket->key = NULL;
				free(current_bucket);
				return;
			}
			previous_bucket = current_bucket;
			current_bucket = current_bucket->next;
		}
	}
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data)
{
	if (hm==NULL) {
		return;
	}
	for (size_t i = 0; i < hm->size; i++)
	{
		//if a bucket has been created at an index, iterate through it
		if (hm->buckets[i])
		{
			Bucket *bucket = hm->buckets[i];
			while (bucket)
			{
				if (destroy_data){
					destroy_data(bucket->data);
				}
				Bucket* temp = bucket;
				bucket = bucket->next;
				free(temp);
			}
		}
	}
	free(hm->buckets);
	free(hm);
}
