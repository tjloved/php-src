--TEST--
Test to ensure const list syntax declaration works
--FILE--
<?php

class Obj
{
    const DECLARE = 'declare', RETURN = 'return', FUNCTION = 'function', USE = 'use';
}
function fn251895676()
{
    echo Obj::DECLARE, PHP_EOL;
    echo Obj::RETURN, PHP_EOL;
    echo Obj::FUNCTION, PHP_EOL;
    echo Obj::USE, PHP_EOL;
    echo Obj::USE, PHP_EOL;
    echo "\nDone\n";
}
fn251895676();
--EXPECTF--
declare
return
function
use
use

Done
