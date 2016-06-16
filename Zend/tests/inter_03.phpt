--TEST--
Testing interface constants with inheritance
--FILE--
<?php

interface a
{
    const b = 2;
}
interface b extends a
{
    const c = self::b;
}
function fn1820582059()
{
    var_dump(b::c, a::b);
}
fn1820582059();
--EXPECT--
int(2)
int(2)
