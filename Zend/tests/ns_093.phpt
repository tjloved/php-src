--TEST--
Group use declarations and whitespace nuances
--FILE--
<?php

// should not throw syntax errors
use Foo\Bar\{A};
use Foo\Bar\{B};
use Foo\Bar\{C};
use Foo\Bar\{D};
function fn586224153()
{
    echo "\nDone\n";
}
fn586224153();
--EXPECTF--

Done