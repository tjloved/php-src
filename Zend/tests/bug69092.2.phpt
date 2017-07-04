--TEST--
Bug #69092-2 (Declare Encoding Compile Check Wrong) - multibyte off
--INI--
zend.multibyte=0
--FILE--
<?php

function foo()
{
    declare (encoding="UTF-8");
}
function fn1245941262()
{
    echo "Hi";
    echo "Bye";
}
fn1245941262();
--EXPECTF--
Warning: declare(encoding=...) ignored because Zend multibyte feature is turned off by settings in %s on line %d

Fatal error: Encoding declaration pragma must be the very first statement in the script in %s on line %d