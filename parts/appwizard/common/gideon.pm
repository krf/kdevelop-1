use File::Copy;
use Getopt::Long;
use IO::Handle;

sub customize
{
        my ( $filename ) = @_;

        open(IN, "<$filename") || die "Could not open for reading: $filename\n";
        open(OUT, ">$filename.tmp") || die "Could not open for writing: $filename.tmp\n";
        while (<IN>) {
              s/\$AUTHOR\$/$AUTHOR/g;
              s/\$EMAIL\$/$EMAIL/g;
              s/\$VERSION\$/$VERSION/g;
              s/\$APPNAMELC\$/$APPNAMELC/g;
              s/\$APPNAMEUC\$/$APPNAMEUC/g;
              s/\$APPNAME\$/$APPNAME/g;
              s/\$LICENSE\$/$LICENSE/g;
              s/\$LICENSEFILE\$/$LICENSEFILE/g;
              print OUT $_;
         }
         close IN;
         close OUT;
         move( "$filename.tmp", $filename );
}


sub install
{
        my ( $srcfilename, $destfilename ) = @_;

        copy( $srcfilename, $destfilename ) 
          || die "Could not copy $srcfilename to $destfilename: $!";
        customize( $destfilename );
}


sub initGideon
{
        autoflush STDOUT 1;

        GetOptions( 'author=s'      => \$AUTHOR,
                    'email=s'       => \$EMAIL,
                    'version=s'     => \$VERSION,
                    'source=s'      => \$src,
                    'dest=s'        => \$dest,
                    'appname=s'     => \$APPNAME,
                    'filetemplate:s'=> \$FILETEMPLATE,
                    'license=s'     => \$LICENSE,
                    'licensefile:s' => \$LICENSEFILE ) || die "Wrong options\n";

        $APPNAMELC = lc $APPNAME;
        $APPNAMEUC = uc $APPNAME;

        print "Making destination directory\n";
        mkdir( "${dest}" );
}


sub installFileTemplate
{
        return if $FILETEMPLATE eq "";

        print "Installing file template\n";
        install( $FILETEMPLATE, "${dest}/.gideon-filetemplate" );
}


sub installLicense
{
        return if $LICENSEFILE eq "";

        print "Installing license file\n";
        install( "${src}/template-common/$LICENSEFILE", "${dest}/$LICENSEFILE" );
}


sub installAdmin
{
        print "Installing admin directory\n";
        copy( "${src}/template-common/admin.tar.gz", "${dest}/admin.tar.gz" );
        chdir( $dest ) || die "Could not chdir to $dest\n";
        system( 'gunzip', '-f', 'admin.tar.gz' );
        system( 'tar', 'xf', 'admin.tar' );
        unlink( "admin.tar" );
}


sub installGNU()
{
        print "Installing GNU coding standard files\n";
        copy( "${src}/template-common/gnu.tar.gz", "${dest}/gnu.tar.gz" );
        chdir( $dest ) || die "Could not chdir to $dest\n";
        system( 'gunzip', '-f', 'gnu.tar.gz' );
        system( 'tar', 'xf', 'gnu.tar' );
        unlink( "gnu.tar" );
        customize( "${dest}/AUTHORS" );
        customize( "${dest}/COPYING" );
        customize( "${dest}/ChangeLog" );
        customize( "${dest}/INSTALL" );
        customize( "${dest}/README" );
        customize( "${dest}/TODO" );
}


sub installDocbook()
{
        print "Installing Docbook template\n";
        mkdir( "${dest}/doc" );
        install( "${src}/template-common/kde-doc-Makefile.am", "${dest}/doc/Makefile.am" );
        mkdir( "${dest}/doc/en" );
        install( "${src}/template-common/kde-doc-en-Makefile.am", "${dest}/doc/en/Makefile.am" );
        install( "${src}/template-common/kde-index.docbook", "${dest}/doc/en/index.docbook" );
}

1;
