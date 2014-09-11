// MyZip.cpp : 定义控制台应用程序的入口点。
//

#include <archive.h>
#include <archive_entry.h>

#define	ASSERT(x)		if(!(x)) {printf("Error " #x "\n");assert(0);}

int myarch_set_entry(const char *filename, size_t  len, struct archive_entry *entry)
{
	time_t	tm;

	archive_entry_set_pathname(entry, filename);
	archive_entry_set_size(entry, len);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_entry_set_mtime(entry, time(&tm), 0);
	return 0;
}

void
write_archive(const char *outname, const char **filename)
{
	struct archive *a;
	struct archive_entry *entry;
	struct stat st;
	char buff[8192];
	int len;
	FILE* fd;

	a = archive_write_new();
	//archive_write_add_filter_gzip(a);
	//archive_write_set_format_pax_restricted(a); // Note 1
	archive_write_set_format_zip(a);
	//archive_write_add_filter_none(a);

	archive_write_open_filename(a, outname);
	while (*filename) {
		stat(*filename, &st);
		entry = archive_entry_new(); // Note 2
		myarch_set_entry( *filename, st.st_size, entry);
		archive_write_header(a, entry);
		fd = fopen(*filename, "rb");
		printf("Read File %s return fd %p\n", *filename, fd);
		while ((len = fread(buff, 1, sizeof(buff), fd)) > 0) {
			printf(" len %d\n", len);
			archive_write_data(a, buff, len);
		}
		fclose(fd);
		archive_entry_free(entry);
		filename++;
	}
	archive_write_close(a); // Note 4
	archive_write_finish(a); // Note 5
}

int main2(int argc, const char **argv)
{
	const char *outname;
	argv++;
	outname = *argv++;
	write_archive(outname, argv);
	return 0;
}