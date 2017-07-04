--TEST--
Testing const names
--FILE--
<?php

const a = 'a';
const A = 'b';
class a
{
    const a = 'c';
    const A = 'd';
}
function fn90782795()
{
    var_dump(a, A, a::a, a::A);
}
fn90782795();
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
