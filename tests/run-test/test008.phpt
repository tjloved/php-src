--TEST--
Error message handling (without ZendOpcache)
--SKIPIF--
<?php
!extension_loaded("Zend Opcache") or die("skip Zend Opcache is loaded");
?>
--INI--
track_errors=1
--FILE--
<?php

function fn711700156()
{
    // If this test fails ask the developers of run-test.php
    //
    // We check the general ini settings which affect error handling
    // and than verify if a message is given by a division by zero.
    // EXPECTF is used here since the error format may change but ut
    // should always contain 'Division by zero'.
    var_dump(ini_get('display_errors'));
    var_dump(ini_get('error_reporting'));
    var_dump(ini_get('log_errors'));
    var_dump(ini_get('track_errors'));
    ini_set('display_errors', "0");
    var_dump(ini_get('display_errors'));
    $error = 1 / 0;
    var_dump($php_errormsg);
}
fn711700156();
--EXPECTF--
Deprecated: Directive 'track_errors' is deprecated in Unknown on line %d
string(1) "1"
string(5) "32767"
string(1) "0"
string(1) "1"
string(1) "0"
string(%d) "%sivision by zer%s"
