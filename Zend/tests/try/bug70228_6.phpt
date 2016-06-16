--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function test($x)
{
    foreach ($x as $v) {
        try {
            return str_repeat("a", 2);
        } finally {
            return 42;
        }
    }
}
function fn1941576191()
{
    var_dump(test([1]));
}
fn1941576191();
--EXPECT--
int(42)
