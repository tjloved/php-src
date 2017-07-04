--TEST--
Bug #66041: list() fails to unpack yielded ArrayAccess object
--FILE--
<?php

function dumpElement()
{
    list($value) = yield;
    var_dump($value);
}
function fn92924306()
{
    $fixedArray = new SplFixedArray(1);
    $fixedArray[0] = 'the element';
    $generator = dumpElement();
    $generator->send($fixedArray);
}
fn92924306();
--EXPECT--
string(11) "the element"
