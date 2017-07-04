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
function fn946422372()
{
    class_alias('a', 'b');
}
fn946422372();
--EXPECTF--
Warning: Cannot declare interface b, because the name is already in use in %s on line %d
