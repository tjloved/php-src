--TEST--
Testing string scanner confirmance
--FILE--
<?php

function fn313648553()
{
    echo "\"\t\\'" . '\\n\\\'a\\\\b\\';
}
fn313648553();
--EXPECT--
"	\'\n\'a\\b\
