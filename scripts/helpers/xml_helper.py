import xml.etree.ElementTree

def register_all_namespaces(filename):
    namespaces = dict([node for _, node in xml.etree.ElementTree.iterparse(filename, events=['start-ns'])])
    uniques = []
    for ns in namespaces:
        if namespaces[ns] not in uniques:
            uniques.append(namespaces[ns])
            xml.etree.ElementTree.register_namespace(ns, namespaces[ns])
            # print(f"registered ns {ns}: {namespaces[ns]}")
        # else:
            # print(f"skipped existing ns {ns}: {namespaces[ns]}")

