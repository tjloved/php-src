--TEST--
Test vfprintf() function : error conditions (wrong argument types)
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--INI--
precision=14
--FILE--
<?php
/* Prototype  : int vfprintf(resource stream, string format, array args)
 * Description: Output a formatted string into a stream 
 * Source code: ext/standard/formatted_print.c
 * Alias to functions: 
 */

// Open handle
$file = 'vfprintf_test.txt';
$fp = fopen( $file, "a+" );

echo "\n-- Testing vfprintf() function with wrong variable types as argument --\n";
var_dump( vfprintf( $fp, array( 'foo %d', 'bar %s' ), 3.55552 ) );

rewind( $fp );
var_dump( stream_get_contents( $fp ) );
ftruncate( $fp, 0 );
rewind( $fp );

var_dump( vfprintf( $fp, "Foo %y fake", "not available" ) );

rewind( $fp );
var_dump( stream_get_contents( $fp ) );
ftruncate( $fp, 0 );
rewind( $fp );

// Close handle
fclose( $fp );

?>
===DONE===
--CLEAN--
<?php

$file = 'vfprintf_test.txt';
unlink( $file );

?>
--EXPECTF--
-- Testing vfprintf() function with wrong variable types as argument --

Warning: vfprintf() expects parameter 2 to be string, array given in %s on line %d
NULL
string(0) ""

Warning: vfprintf() expects parameter 3 to be array, string given in %s on line %d
NULL
string(0) ""
===DONE===
