--TEST--
Bug #65254 (Exception not catchable when exception thrown in autoload with a namespace)
--FILE--
<?php

function __autoload($class)
{
    eval("namespace ns_test; class test {}");
    throw new \Exception('abcd');
}
function fn240845679()
{
    try {
        \ns_test\test::go();
    } catch (Exception $e) {
        echo 'caught';
    }
}
fn240845679();
--EXPECT--
caught
