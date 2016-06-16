--TEST--
Testing string scanner confirmance
--FILE--
<?php

function fn1759638701()
{
    echo "\"\t\\'" . '\\n\\\'a\\\\b\\';
}
fn1759638701();
--EXPECT--
"	\'\n\'a\\b\
