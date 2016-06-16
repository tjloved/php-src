--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function foo()
{
    try {
        return str_repeat("a", 2);
    } finally {
        return str_repeat("b", 2);
    }
}
function fn548573295()
{
    var_dump(foo());
}
fn548573295();
--EXPECT--
string(2) "bb"
