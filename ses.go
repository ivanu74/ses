package ses

import (
	"errors"
	"unsafe"
)

// #cgo CFLAGS: -I./include/
// #cgo ppc64le LDFLAGS: -L./ppc64/
// #cgo amd64 LDFLAGS: -L./amd64/
// #cgo LDFLAGS: -lsgutils2
// #include <stdlib.h>
// #include "jbod.h"
import "C"

// GetProfile - return 3 strings separate by '\n'.
// 1st string - vendor
// 2nd string - product
// 3rd string - revision
func GetProfile(name string) []byte {
	out := C.CBytes(make([]byte, 1024*1024))
	defer C.free(out)
	device := C.CString(name)
	defer C.free(unsafe.Pointer(device))

	size := C.get_profile(device, (*C.char)(out))
	s := C.GoBytes(out, size)

	return s
}

// GetSerial - return product serial number from page 0x80.
func GetSerial(name string) []byte {
	out := C.CBytes(make([]byte, 1024*1024))
	defer C.free(out)
	device := C.CString(name)
	defer C.free(unsafe.Pointer(device))

	size := C.get_serial(device, (*C.char)(out))
	s := C.GoBytes(out, size)

	return s
}

// GetSasInfo - return strings separated by '\n'.
// 1st string - device Slot
// 2nd string - device SAS Address
// 3rd string - device Attached SAS Address
// 4-6 strings other device etc.
// there is only devices that have SAS address not null.
func GetSasInfo(name string) []byte {
	out := C.CBytes(make([]byte, 1024*1024))
	defer C.free(out)
	device := C.CString(name)
	defer C.free(unsafe.Pointer(device))

	size := C.get_sas_info(device, (*C.char)(out))
	s := C.GoBytes(out, size)

	return s
}

// SetHddLed - on/off hdd led, pointed by name and array device id
// name - name of jboe, example /dev/sg4
// id - number of array device or slot, example 61
// op - only 0 or 1. 0 - off, 1 - on
// return error - if operation is not success
func SetHddLed(name string, id int, op int) error {
	if op != 0 && op != 1 {
		return errors.New("wrong parameter op")
	}
	device := C.CString(name)
	defer C.free(unsafe.Pointer(device))

	if ret := C.set_hdd_led(device, C.int(id), C.int(op)); int(ret) != id {
		return errors.New("can't set hdd led")
	}
	return nil
}
