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
function fn868686963()
{
    var_dump(a, A, a::a, a::A);
}
fn868686963();
--EXPECT--
string(1) "a"
string(1) "b"
string(1) "c"
string(1) "d"
