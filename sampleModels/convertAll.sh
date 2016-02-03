for FILENAME in $(find . -name '*.dae')
do
    basename=`echo $FILENAME | sed 's/.*\/\([^.]*\)\..*/\1/'`
    dirname=`echo $FILENAME | sed 's/\(.*\)\/.*/\1/'`

    dir="$dirname/../glTF"
    [[ -d $dir ]] || mkdir $dir
    collada2gltf -d -f $FILENAME -o $dir/$basename.gltf

    dir="$dirname/../glTF-Embedded"
    [[ -d $dir ]] || mkdir $dir
    collada2gltf -d -e -f $FILENAME -o $dir/$basename.gltf

    dir="$dirname/../glTF-MaterialsCommon"
    [[ -d $dir ]] || mkdir $dir
    collada2gltf -d -k -f $FILENAME -o $dir/$basename.gltf

    #dir="$dirname/../glTF-Binary"
    #[[ -d $dir ]] || mkdir $dir
    #ModelConverter -b -i $FILENAME -o $dir
done
