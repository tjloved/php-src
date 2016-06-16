--TEST--
Bug #31341 (escape on curly inconsistent)
--FILE--
<?php

function fn512356545()
{
    $a = array("\$     \\{    ", "      \\{   \$", "      \\{\$   ", "      \$\\{   ", "      \$\\{  ", "      \\{\$  ", "\$    \\{    ", "      \\{  \$", "%     \\{    ");
    foreach ($a as $v) {
        echo "'{$v}'\n";
    }
}
fn512356545();
--EXPECT--
'$     \{    '
'      \{   $'
'      \{$   '
'      $\{   '
'      $\{  '
'      \{$  '
'$    \{    '
'      \{  $'
'%     \{    '
