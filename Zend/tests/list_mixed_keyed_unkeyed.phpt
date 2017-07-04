--TEST--
list() with both keyed and unkeyed elements
--FILE--
<?php

function fn2114336782()
{
    $contrivedKeyedAndUnkeyedArrayExample = [0, 1 => 1, "foo" => "bar"];
    list($zero, 1 => $one, "foo" => $foo) = $contrivedKeyedAndUnkeyedArrayExample;
}
fn2114336782();
--EXPECTF--
Fatal error: Cannot mix keyed and unkeyed array entries in assignments in %s on line %d
