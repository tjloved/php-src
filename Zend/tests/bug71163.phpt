--TEST--
Bug #71163 (Segmentation Fault (cleanup_unfinished_calls))
--FILE--
<?php

function test2()
{
    try {
        Test::foo();
    } catch (Exception $e) {
        echo "okey";
    }
}
function test()
{
    test2();
}
function fn453883382()
{
    spl_autoload_register(function ($name) {
        eval("class {$name} extends Exception { public static function foo() {}}");
        throw new Exception("boom");
    });
    test();
}
fn453883382();
--EXPECT--
okey
