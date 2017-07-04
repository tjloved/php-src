--TEST--
Checking trait_exists(): interface, trait, abstract and final class
--FILE--
<?php

interface a
{
}
abstract class b
{
}
final class c
{
}
trait d
{
}
function fn875194355()
{
    var_dump(trait_exists('a'));
    var_dump(trait_exists('b'));
    var_dump(trait_exists('c'));
    var_dump(trait_exists('d'));
}
fn875194355();
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
