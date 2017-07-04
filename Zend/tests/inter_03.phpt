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
function fn256773069()
{
    var_dump(b::c, a::b);
}
fn256773069();
--EXPECT--
int(2)
int(2)
