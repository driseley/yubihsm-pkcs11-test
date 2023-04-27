// Inspired by https://github.com/Open-TEE/tests/blob/master/pkcs11/pkcs11_test_app.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>
#include <pkcs11/pkcs11.h>
#include <arpa/inet.h>

static CK_FUNCTION_LIST_PTR func_list;
static char user_pin[12] = "0001password";

static int find_object(CK_SESSION_HANDLE session,
		       CK_OBJECT_HANDLE **hObject,
		       CK_ULONG *find_object_count,
		       CK_ATTRIBUTE *find_template,
		       CK_ULONG find_template_count)
{
	CK_OBJECT_HANDLE *found_objects, *realloc_found_objects;
	uint32_t i = 0, object_fetcs_count = 10,
			block_size = sizeof(CK_OBJECT_HANDLE) * object_fetcs_count;
	CK_ULONG ulObjectCount;
	CK_RV ret;

	*find_object_count = 0;

	found_objects = calloc(1, block_size);
	if (found_objects == NULL) {
		printf("Out of memory");
		return 1;
	}

	/* Init find operation */
	ret = func_list->C_FindObjectsInit(session, find_template, find_template_count);
	if (ret != CKR_OK) {
		printf("Failed to init find object %lu : 0x%x", ret, (uint32_t)ret);
		free(found_objects);
		return 1;
	}

	while (1) {
		ret = func_list->C_FindObjects(session, found_objects + (i * block_size),
					       object_fetcs_count, &ulObjectCount);
		if (ret != CKR_OK) {
			printf("Failed to find objects %lu : 0x%x", ret, (uint32_t)ret);
			free(found_objects);
			return 1;
		}

		*find_object_count += ulObjectCount;

		if (ulObjectCount == block_size) {

			++i;
			realloc_found_objects = realloc(found_objects, i * block_size);
			if(realloc_found_objects == NULL) {
				printf("Out of memory");
				free(found_objects);
				return 1;
			}

			found_objects = realloc_found_objects;

		} else {
			break;
		}
	}

	ret = func_list->C_FindObjectsFinal(session);
	if (ret != CKR_OK) {
		printf("Failed to finalize objects find %lu : 0x%x", ret, (uint32_t)ret);
		free(found_objects);
		return 1;
	}

	if (*found_objects == 0) {
		free(found_objects);
		found_objects = NULL;
	}
	*hObject = found_objects;
	return 0;
}

static void find_ojbect_by_id_byte(CK_SESSION_HANDLE session, uint8_t id)
{
	CK_OBJECT_HANDLE *hObject;
	CK_ULONG ulObjectCount;

	CK_OBJECT_CLASS obj_class = CKO_PUBLIC_KEY;
	CK_ATTRIBUTE attribs[2] = {
		{CKA_CLASS, &obj_class, sizeof(obj_class)},
		{CKA_ID, &id, sizeof(id)}
	};

	printf("Searching for object with id(byte) %d\n", id);

	if (find_object(session, &hObject, &ulObjectCount, attribs, 2)) {
		return;
	} else {
		printf("Found %lu objects\n", ulObjectCount);
		free(hObject);
	}
    
    return;
}

static void find_ojbect_by_id_short(CK_SESSION_HANDLE session, uint16_t id)
{
	CK_OBJECT_HANDLE *hObject;
	CK_ULONG ulObjectCount;

	// Convert the id to network byte order
	uint16_t nboId = htons(id);

	CK_OBJECT_CLASS obj_class = CKO_PUBLIC_KEY;
	CK_ATTRIBUTE attribs[2] = {
		{CKA_CLASS, &obj_class, sizeof(obj_class)},
		{CKA_ID, &nboId, sizeof(nboId)}
	};

	printf("Searching for object with id(short) %u\n", id);

	if (find_object(session, &hObject, &ulObjectCount, attribs, 2)) {
		return;
	} else {
		printf("Found %lu objects\n", ulObjectCount);
		free(hObject);
	}
    
    return;
}

static void find_ojbect_by_id_long(CK_SESSION_HANDLE session, uint32_t id)
{
	CK_OBJECT_HANDLE *hObject;
	CK_ULONG ulObjectCount;

	// Convert the id to network byte order
	uint32_t nboId = htonl(id);

	CK_OBJECT_CLASS obj_class = CKO_PUBLIC_KEY;
	CK_ATTRIBUTE attribs[2] = {
		{CKA_CLASS, &obj_class, sizeof(obj_class)},
		{CKA_ID, &nboId, sizeof(nboId)}
	};

	printf("Searching for object with id(long) %lu\n", id);

	if (find_object(session, &hObject, &ulObjectCount, attribs, 2)) {
		return;
	} else {
		printf("Found %lu objects\n", ulObjectCount);
		free(hObject);
	}
    
    return;
}

int main(int argc, char **argv) {
	CK_SESSION_HANDLE session;
	CK_INFO info;
	CK_RV ret;
	CK_SLOT_ID available_slots[1];
	CK_ULONG num_slots = 1;

    printf("START: pkcs11 test app\n");

    ret = C_GetFunctionList(&func_list);
	if (ret != CKR_OK || func_list == NULL) {
		printf("Failed to get function list: %ld\n", ret);
		return 0;
	} else {
		printf("Got function list\n");
	}

    ret = func_list->C_Initialize(NULL);
	if (ret != CKR_OK) {
		printf("Failed to initialize the library: %ld\n", ret);
		return 0;
	} else {
		printf("Intialised\n");
	}

	ret = func_list->C_GetSlotList(1, available_slots, &num_slots);
	if (ret != CKR_OK) {
		printf("Failed to get the available slots: %ld", ret);
		return 0;
	} else {
		printf("Found %ld slots\n", num_slots);
	}

	ret = func_list->C_OpenSession(available_slots[0], CKF_RW_SESSION | CKF_SERIAL_SESSION,
				       NULL, NULL, &session);
	if (ret != CKR_OK) {
		printf("Failed to Open session the library: 0x%x", (uint32_t)ret);
		return 0;
	} else {
		printf("Session opened\n");
	}

	ret = func_list->C_Login(session, CKU_USER, (CK_BYTE_PTR)user_pin, sizeof(user_pin));
	if (ret != CKR_OK) {
		printf("Failed to login: 0x%x", (uint32_t)ret);
		return 0;
	} else {
		printf("Logged in\n");
	}

	// Change the ojbect id as appropriate
	find_ojbect_by_id_byte(session, 200);
	find_ojbect_by_id_short(session, 200);
	find_ojbect_by_id_long(session, 200);

	find_ojbect_by_id_short(session, 65000);
	find_ojbect_by_id_long(session, 65000);
	
    func_list->C_CloseSession(session);
	printf("Session closed\n");

	ret = func_list->C_Finalize(NULL);
	if (ret != CKR_OK) {
		printf("Failed to Finalize the library: %ld", ret);
		return 0;
	} else {
		printf("Finalized\n");
	}

	printf("END: pkcs11 test app\n\n");
}