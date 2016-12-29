/* Soma - Copyright (C) 2003-7 bakunin - Andrea Marchesini 
 *                                       <bakunin@autistici.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 * 02110-1301, USA.
 *
 * This program is released under the GPL with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 */

#ifndef __SOMA_CODE_H__
#define __SOMA_CODE_H__

#define SOMA_STR_WELCOME		"100 - Welcome to soma daemon\n"
#define SOMA_CODE_WELCOME		100

#define SOMA_STR_OK			"100 - Ok\n"
#define SOMA_CODE_OK			100

#define SOMA_STR_ERR_PASS		"101 - Password error\n"
#define SOMA_CODE_ERR_PASS		101

#define SOMA_STR_ERR_CMD		"102 - Command error\n"
#define SOMA_CODE_ERR_CMD		102

#define SOMA_STR_ERR			"103 - Generic Error\n"
#define SOMA_CODE_ERR			103

#define SOMA_STR_READ_PL		"104 - Read Palinsesto Request\n"
#define SOMA_CODE_READ_PL		104

#define SOMA_STR_READ_DIR		"105 - Read Directory Request\n"
#define SOMA_CODE_READ_DIR		105

#define SOMA_STR_NEW_PL			"106 - Switch to a New Palinsesto Request\n"
#define SOMA_CODE_NEW_PL		106

#define SOMA_STR_OLD_PL			"108 - Switch to Old Palinsesto Request\n"
#define SOMA_CODE_OLD_PL		108

#define SOMA_STR_GET_PL			"109 - Get the current Palinsesto\n"
#define SOMA_CODE_GET_PL		109

#define SOMA_STR_PASSWORD		"110 - Password Check\n"
#define SOMA_CODE_PASSWORD		110

#define SOMA_STR_STATUS			"111 - Get status\n"
#define SOMA_CODE_STATUS		111

#define SOMA_STR_QUIT			"112 - Quit Request\n"
#define SOMA_CODE_QUIT			112

#define SOMA_STR_STOP			"113 - Stop Request\n"
#define SOMA_CODE_STOP			113

#define SOMA_STR_START			"114 - Start Request\n"
#define SOMA_CODE_START			114

#define SOMA_STR_RUNNING		"115 - Get Running status\n"
#define SOMA_CODE_RUNNING		115

#define SOMA_STR_NEXT_ITEM_SET		"107 - Set a NextItem Request\n"
#define SOMA_CODE_NEXT_ITEM_SET		107

#define SOMA_STR_NEXT_ITEM_REMOVE	"116 - Remove a NextItem Request\n"
#define SOMA_CODE_NEXT_ITEM_REMOVE	116

#define SOMA_STR_NEXT_ITEM_LIST		"117 - Get List of NextItem Request\n"
#define SOMA_CODE_NEXT_ITEM_LIST	117

#define SOMA_STR_NEXT_ITEM_PATH_SET	"118 - Set a Distribuited NextItem Request\n"
#define SOMA_CODE_NEXT_ITEM_PATH_SET	118

#define SOMA_STR_TIME			"119 - Get Daemon Time Request\n"
#define SOMA_CODE_TIME			119

#define SOMA_STR_GET_PATH		"120 - Get Data From Path\n"
#define SOMA_CODE_GET_PATH		120

#define SOMA_STR_SKIP			"121 - Skip Current Item\n"
#define SOMA_CODE_SKIP			121

#define SOMA_STR_DEFAULT_PL		"122 - Set the current Palinsesto as Default\n"
#define SOMA_CODE_DEFAULT_PL		122

#define SOMA_STR_GET_OLD_PL		"123 - Get Old Palinsesto File\n"
#define SOMA_CODE_GET_OLD_PL		123

#define SOMA_STR_NEXT_ITEM_SET_SPOT	"124 - Set Spot in NextItem iteration Request\n"
#define SOMA_CODE_NEXT_ITEM_SET_SPOT	124

#define SOMA_STR_NEXT_ITEM_GET_SPOT	"125 - Get Spot in NextItem iteration Request\n"
#define SOMA_CODE_NEXT_ITEM_GET_SPOT	125

#define SOMA_STR_READ_SPOT		"126 - Read Spot Request\n"
#define SOMA_CODE_READ_SPOT		126

#define SOMA_STR_NEW_SPOT		"128 - Switch to a New Spot Request\n"
#define SOMA_CODE_NEW_SPOT		128

#define SOMA_STR_OLD_SPOT		"129 - Switch to a Old Spot Request\n"
#define SOMA_CODE_OLD_SPOT		129

#define SOMA_STR_GET_SPOT		"130 - Get the current Spot\n"
#define SOMA_CODE_GET_SPOT		130

#define SOMA_STR_DEFAULT_SPOT		"131 - Set the current Spot as Default\n"
#define SOMA_CODE_DEFAULT_SPOT		131

#define SOMA_STR_GET_OLD_SPOT		"132 - Get Old Spot File\n"
#define SOMA_CODE_GET_OLD_SPOT		132

#define SOMA_STR_ERR_DATA		"133 - No data\n"
#define SOMA_CODE_ERR_DATA		133

#define SOMA_STR_GET_ITEM		"134 - Get Item\n"
#define SOMA_CODE_GET_ITEM		134

#define SOMA_STR_GET_STAT		"135 - Get Stat\n"
#define SOMA_CODE_GET_STAT		135

#define SOMA_STR_GET_STAT_DIR		"136 - Get Stat Dir\n"
#define SOMA_CODE_GET_STAT_DIR		136

#define SOMA_STR_GET_STAT_PATH		"137 - Get Stat Path\n"
#define SOMA_CODE_GET_STAT_PATH		137

#define SOMA_STR_GET_STAT_DIR_PATH	"138 - Get Stat Dir Path\n"
#define SOMA_CODE_GET_STAT_DIR_PATH	138

#define SOMA_STR_GET_ITEM_LIST		"139 - Get Item List\n"
#define SOMA_CODE_GET_ITEM_LIST		139

#define SOMA_STR_GET_SPOT_LIST		"140 - Get Spot List\n"
#define SOMA_CODE_GET_SPOT_LIST		140

#define SOMA_STR_GET_PL_NAME		"141 - Get Current Palinsesto\n"
#define SOMA_CODE_GET_PL_NAME		141

#define SOMA_STR_GET_TIME_PLAY		"142 - Get Time Play\n"
#define SOMA_CODE_GET_TIME_PLAY		142

#define SOMA_STR_REMOVE_ITEM		"143 - Get Remove Item\n"
#define SOMA_CODE_REMOVE_ITEM		143

#define SOMA_STR_REMOVE_SPOT		"144 - Get Remove Spot\n"
#define SOMA_CODE_REMOVE_SPOT		144

#define SOMA_STR_GET_FOLLOW_ITEM	"145 - Get Follow Item\n"
#define SOMA_CODE_GET_FOLLOW_ITEM	145

#define SOMA_STR_SKIP_FOLLOW_ITEM	"146 - Skip Follow Item\n"
#define SOMA_CODE_SKIP_FOLLOW_ITEM	146

#define SOMA_STR_PAUSE			"147 - Pause\n"
#define SOMA_CODE_PAUSE			147

#define SOMA_STR_UNPAUSE		"148 - Remove Pause\n"
#define SOMA_CODE_UNPAUSE		148

#define SOMA_STR_GET_PAUSE		"149 - Get Pause Status\n"
#define SOMA_CODE_GET_PAUSE		149

#define SOMA_STR_GET_STOP		"150 - Get Stop Status\n"
#define SOMA_CODE_GET_STOP		150

/* Type of codes */
#define SOMA_STR_FILES		"files"
#define SOMA_CODE_FILES		0

#define SOMA_STR_STREAM		"stream"
#define SOMA_CODE_STREAM	1

#define SOMA_STR_SILENCE	"silence"
#define SOMA_CODE_SILENCE	2

#define SOMA_STR_MODULE		"module"
#define SOMA_CODE_MODULE	3

/* Debug */
#define SOMA_DEBUG_NO		0
#define SOMA_DEBUG_ERR		1
#define SOMA_DEBUG_WARN		2
#define SOMA_DEBUG_INFO		3
#define SOMA_DEBUG_ALL		4

#define SOMA_DEBUG_NO_ERR	-1
#define SOMA_DEBUG_NO_WARN	-2
#define SOMA_DEBUG_NO_INFO	-3
#define SOMA_DEBUG_NO_ALL	-4

/* item/spot structs */
#define SOMA_PLAY_STREAM 	1
#define SOMA_PLAY_FILES 	0

#define SOMA_DEFAULT_ITEM	0
#define SOMA_DEFAULT_SPOT	1

#define SOMA_PALINSESTO_ITEM	2
#define SOMA_PALINSESTO_SPOT	3

#define SOMA_ITEM		0
#define SOMA_SPOT		1

#define SOMA_OPT_ITEM		0
#define SOMA_OPT_STREAM		1

#endif
