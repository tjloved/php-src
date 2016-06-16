--TEST--
implementing the same interface twice
--FILE--
<?php

interface foo
{
}
class bar implements foo, foo
{
}
function fn2083730103()
{
    echo "Done\n";
}
fn2083730103();
--EXPECTF--	
Fatal error: Class bar cannot implement previously implemented interface foo in %s on line %d
