--TEST--
Test bitwise AND, OR, XOR, NOT and logical NOT in INI via error_reporting
--INI--
error_reporting = E_ALL & E_NOTICE | E_STRICT ^ E_DEPRECATED & ~E_WARNING | !E_ERROR
--FILE--
<?php

function fn2034928119()
{
    echo ini_get('error_reporting');
}
fn2034928119();
--EXPECT--
10248
