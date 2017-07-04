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
function fn1856362369()
{
    var_dump(foo());
}
fn1856362369();
--EXPECT--
string(2) "bb"
