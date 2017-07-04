--TEST--
Bug #70228 (memleak if return hidden by throw in finally block)
--FILE--
<?php

function test()
{
    try {
        return str_repeat("a", 2);
    } finally {
        throw new Exception("ops");
    }
}
function fn266170824()
{
    try {
        test();
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}
fn266170824();
--EXPECT--
ops
