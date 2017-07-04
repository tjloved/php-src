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
    abstract function bar($x, $y = 0);
}
function fn869394697()
{
    echo "DONE";
}
fn869394697();
--EXPECT--	
DONE
