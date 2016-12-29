<?
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

dl("soma.so");

function check_error($k) {

//  if(soma_error($k)==SOMA_ERR_CONNECT) echo "..."

  if(soma_error($k)) {
    echo soma_error_string($k)."\n";
    exit;
  }
}

$k=soma_open_tcp("localhost", 0, "passwd", 0);
check_error($k);

echo "STATUS: " . soma_status($k) . "\n";
echo "PALINSESTO: " . soma_palinsesto_name($k) . "\n";
echo "ITEM: " . soma_get_item($k) . "\n";

echo "PLAY TIME:";
for ($i=0;$i<10;$i++) {
  echo soma_get_time_play($k). " ";
  sleep(1);
}

echo "Next item list:\n";
$l=soma_nextitem_list($k);
for ($i=0;$i<count($l);$i++) {
  echo($l[$i])."\n";
}

echo "Read palinsesto... ";
if (soma_read_palinsesto($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Read spot... ";
if (soma_read_spot($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Read directory... ";
if (soma_read_directory($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Get palinsesto....\n";
$pl=soma_get_palinsesto($k);
$fl=fopen("soma.data", "w");
fputs($fl, $pl);
fclose($fl);

$fl=file("soma.data");
for($i=0;$i<count($fl);$i++) echo $fl[$i];

echo "Change palinsesto... ";
if (soma_new_palinsesto_file($k, "soma.data")) echo "Error!\n";
else echo "Ok!\n";

echo "Skip to old palinsesto... ";
if (soma_old_palinsesto($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Get Old palinsesto....\n";
$pl=soma_get_old_palinsesto($k);

echo "Change with the old palinsesto... ";
if (soma_new_palinsesto_buffer($k, $pl)) echo "Error!\n";
else echo "Ok!\n";

echo "Skip to old palinsesto... ";
if (soma_old_palinsesto($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Get spot....\n";
$pl=soma_get_spot($k);
$fl=fopen("soma.data", "w");
fputs($fl, $pl);
fclose($fl);

$fl=file("soma.data");
for($i=0;$i<count($fl);$i++) echo $fl[$i];

echo "Change spot... ";
if (soma_new_spot_file($k, "soma.data")) echo "Error!\n";
else echo "Ok!\n";

echo "Skip to old spot... ";
if (soma_old_spot($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Get Old spot....\n";
$pl=soma_get_old_spot($k);

echo "Change with the old spot... ";
if (soma_new_spot_buffer($k, $pl)) echo "Error!\n";
else echo "Ok!\n";

echo "Skip to old spot... ";
if (soma_old_spot($k)) echo "Error!\n";
else echo "Ok!\n";

echo "Password Checking... ";
if (soma_check_password($k)) echo "Error!\n";
else echo "Ok!\n";

for($i=0;$i<10;$i++) {
  echo "Stop...\n";
  soma_stop($k,$i);
  sleep(1);
  echo "Start...\n";
  soma_start($k);
  sleep(1);
}

echo "Somad is " . (soma_running($k) ? "" : "not ") . "running\n";
echo "Somad time: " . soma_time($k) . "\n";

echo "Distribuited Path:\n";
$l=soma_get_path($k, "/");
for ($i=0;$i<count($l);$i++) {
  echo $l[$i] . "\n";

  if(substr($l[$i],-1) == '/') {
    $l2=soma_get_path($k, "/".$l[$i]);
    for($j=0;$j<count($l2);$j++) {
      if(substr($l2[$j],-1) == '/') {
        $data = soma_get_stat_dir_path($k, "/".$l[$i].$l2[$j]);
	echo "\t".$l2[$j]." ".$data["duration"]."\n";
      }
    }
  }

  else {
    $data = soma_get_stat_path($k, "/".$l[$i]);
    echo "\t".$l[$i]." ".$data["duration"]."\n";
  }
}

echo "Absolute path:\n";
$data = soma_get_stat_dir_path($k, "/home");
echo "Duration of /home: ".$data["duration"]."\n";

echo "List of Item:\n";
$data=soma_get_item_list($k);
for ($i=0;$i<count($data);$i++) 
  echo $data[$i]."\n";

echo "List of Spot:\n";
$data=soma_get_spot_list($k);
for ($i=0;$i<count($data);$i++) 
  echo $data[$i]."\n";

echo "Remove Item:\n";
for ($i=0;$i<10;$i++) {
  soma_remove_item($k, 0);

  $a=soma_get_item_list($k);

  for($j=0;$j<(count($a) < 10 ? count($a) : 10);$j++)
    echo $a[$j]."\n";

  echo "---";
}

echo "Remove Spot:\n";
for ($i=0;$i<10;$i++) {
  soma_remove_spot($k, 0);

  $a=soma_get_spot_list($k);

  for($j=0;$j<(count($a) < 10 ? count($a) : 10);$j++)
    echo $a[$j]."\n";

  echo "---";
}

#soma_quit($k); 
soma_free($k);
?>
