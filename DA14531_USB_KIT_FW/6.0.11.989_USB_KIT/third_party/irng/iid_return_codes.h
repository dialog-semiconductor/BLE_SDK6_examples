/*! \copyright Copyright 2018 Intrinsic ID B.V. All rights reserved.\n
    This text contains proprietary, confidential information of Intrinsic ID B.V.,
    and may be used, copied, distributed and/or published only pursuant to the
    terms of a valid license agreement with Intrinsic ID B.V.\n
    This copyright notice must be retained as part of this text at all times.
*/

#ifndef _IID_RETURN_CODES_H_
#define _IID_RETURN_CODES_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*! \defgroup IRNG ReturnCodes
    \ingroup Irng
    \brief The possible return codes returned by IRNG function.
*/

/*@{*/

/*! \brief Successful execution
    \details Value indicating the successful execution of the called function.
*/
#define IID_SUCCESS                       0x00

/*! \brief Generic return codes base
    \details Value used internally as the base value for all generic return codes.
*/
#define IID_RETURN_BASE                   0x01

/******************************* Generic Return Codes *******************************/
/*! \brief Function call not allowed in the current state
    \details Value indicating that the given function call is not allowed in the current
             state of the IRNG module.
*/
#define IID_NOT_ALLOWED                  (IID_RETURN_BASE + 0x00)

/*! \brief Invalid function parameters
    \details Value indicating that at least one of the parameters passed as argument
             to the function call has an invalid form and/or content.
*/
#define IID_INVALID_PARAMETERS           (IID_RETURN_BASE + 0x01)

/************************** IRNG Specific Return Codes **************************/
/*! \brief Error startup data
    \details Value indicating that the appointed SRAM address does not contain
             qualitative start-up data which can be used as an SRAM PUF by IRNG.
             Re-power the device and try again.
*/
#define IID_ERROR_SRAM_PUF_DATA                  (0x20)

/*! \brief Insufficient SRAM Blocks
    \details Value indicating that number of sram blocks provided is not sufficient for
            irng to work efficiently. Use the \ref IID_IRNG_MINIMUM_SRAM_PUF_BLOCKS macro
          for minimum number of sram blocks required.
*/
#define IID_ERROR_INSUFFICIENT_SRAM_BLOCKS      (0x21)

/*! \brief AES engine timeout
    \details Value indicating that AES engine processing time exceeds the time limit
    set for execution of the data. Please check if it is initialized/configured.
*/
#define IID_ERROR_AES_TIMEOUT                   (0x22)

/*! \brief AES engine failed
    \details Value indicating that AES engine run is unsuccessful.
*/
#define IID_ERROR_AES_FAILED                    (0x23)

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /* _IID_RETURN_CODES_H_ */
