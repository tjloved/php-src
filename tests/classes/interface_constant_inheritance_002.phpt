--TEST--
Ensure a class may not shadow a constant inherited from an interface. 
--FILE--
<?php

interface I
{
    const FOO = 10;
}
class C implements I
{
    const FOO = 10;
}
function fn1003995385()
{
    echo "Done\n";
}
fn1003995385();
--EXPECTF--

Fatal error: Cannot inherit previously-inherited or override constant FOO from interface I in %s on line %d
