--TEST--
Test bitwise AND, OR, XOR, NOT and logical NOT in INI via error_reporting
--INI--
error_reporting = E_ALL & E_NOTICE | E_STRICT ^ E_DEPRECATED & ~E_WARNING | !E_ERROR
--FILE--
<?php

function fn1093642967()
{
    echo ini_get('error_reporting');
}
fn1093642967();
--EXPECT--
10248
