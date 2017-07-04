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
function fn62406239()
{
    var_dump(test());
}
fn62406239();
--EXPECT--
int(42)
