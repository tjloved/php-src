--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function test()
{
    try {
        throw new Exception(1);
    } finally {
        try {
            throw new Exception(2);
        } finally {
            return 42;
        }
    }
}
function fn1248692730()
{
    var_dump(test());
}
fn1248692730();
--EXPECT--
int(42)
