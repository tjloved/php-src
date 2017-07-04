--TEST--
Bug #73337 (try/catch not working with two exceptions inside a same operation)
--FILE--
<?php

class d
{
    function __destruct()
    {
        throw new Exception();
    }
}
function fn78330298()
{
    try {
        new d() + new d();
    } catch (Exception $e) {
        print "Exception properly caught\n";
    }
}
fn78330298();
--EXPECTF--
Notice: Object of class d could not be converted to int in %sbug73337.php on line %d

Notice: Object of class d could not be converted to int in %sbug73337.php on line %d
Exception properly caught
