--TEST--
Trying use an existing alias name in class declaration
--FILE--
<?php

interface a
{
}
class b
{
}
function fn935919253()
{
    class_alias('a', 'b');
}
fn935919253();
--EXPECTF--
Warning: Cannot declare interface b, because the name is already in use in %s on line %d
