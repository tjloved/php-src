--TEST--
Allow abstract function override
--FILE--
<?php

abstract class A
{
    abstract function bar($x);
}
abstract class B extends A
{
    abstract function bar($x);
}
function fn290235609()
{
    echo "DONE";
}
fn290235609();
--EXPECT--	
DONE
